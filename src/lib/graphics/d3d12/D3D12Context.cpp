#include "graphics/d3d12/D3D12Context.hpp"
#include <cassert>

APILearning::D3D12Context::D3D12Context(HWND windowHandle, uint32_t width, uint32_t height, uint32_t numBackBuffers, uint32_t numFramesInFlight) :
	m_BackBuffersAmount(numBackBuffers), m_FramesInFlightAmount(numFramesInFlight)
{
	m_ClearColor[0] = 1.0f;
	m_ClearColor[1] = 140.0f/255.0f;
	m_ClearColor[2] = .0f;
	m_ClearColor[3] = 1.0f;
	HRESULT hr;

	CreateDevice();
	CreateHeapDescriptor();
	CreateQueueDescriptor();
	CreateFrameContext();
	CreateCommandList();
	CreateFence();
	CreateSwapChain(windowHandle);
	CreateRenderTargetView();
	CreateViewport(width, height);
}

APILearning::D3D12Context::~D3D12Context()
{
	delete[] m_RenderTargetResource;
	delete[] m_FrameContext;
	delete[] m_RenderTargetDescriptor;
}

void APILearning::D3D12Context::Update()
{
	//TODO split command list in reset, send commands and resolve
	m_CommandList->ClearRenderTargetView(m_RenderTargetDescriptor[m_CurrentFrame.BackBufferIndex], m_ClearColor, 0, NULL);
	m_CommandList->OMSetRenderTargets(1, &m_RenderTargetDescriptor[m_CurrentFrame.BackBufferIndex], FALSE, NULL);
	m_CommandList->SetDescriptorHeaps(1, m_SourceDescHeap.GetAddressOf());
}

void APILearning::D3D12Context::SetClearColor(float r, float g, float b, float a)
{
	m_ClearColor[0] = r;
	m_ClearColor[1] = g;
	m_ClearColor[2] = b;
	m_ClearColor[3] = a;
}

void APILearning::D3D12Context::ReceiveCommands()
{
	
	m_CurrentFrame.Barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	m_CurrentFrame.Barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	m_CurrentFrame.Barrier.Transition.pResource = m_RenderTargetResource[m_CurrentFrame.BackBufferIndex].Get();
	m_CurrentFrame.Barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	m_CurrentFrame.Barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
	m_CurrentFrame.Barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
	m_CommandList->Reset(m_CurrentFrame.FrameContext->CommandAllocator.Get(), NULL);
	m_CommandList->ResourceBarrier(1, &m_CurrentFrame.Barrier);
}

void APILearning::D3D12Context::DispatchCommands()
{
	m_CurrentFrame.Barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	m_CurrentFrame.Barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
	m_CommandList->ResourceBarrier(1, &m_CurrentFrame.Barrier);
	m_CommandList->Close();
	m_CommandQueue->ExecuteCommandLists(1, (ID3D12CommandList* const*)m_CommandList.GetAddressOf());
}

void APILearning::D3D12Context::NewFrame()
{
	m_CurrentFrame.NextFrameIndex = m_FrameIndex + 1;
	m_FrameIndex = m_CurrentFrame.NextFrameIndex;

	UpdateFrameContext(&m_CurrentFrame.FenceValue, &m_CurrentFrame.FrameContext, &m_CurrentFrame.BackBufferIndex, m_CurrentFrame.NextFrameIndex);
}

void APILearning::D3D12Context::EndFrame()
{
	UpdateFence(&m_CurrentFrame.FrameContext, &m_CurrentFrame.FenceValue);
}

void APILearning::D3D12Context::Present()
{
	m_SwapChain->Present(1, 0);
}

void APILearning::D3D12Context::Draw(uint32_t elements)
{
	m_CommandList->RSSetViewports(1, &m_Viewport);
	m_CommandList->RSSetScissorRects(1, &m_ScissorRect);
	m_CommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	m_CommandList->DrawIndexedInstanced(elements, 1, 0, 0, 0);
}

ID3D12Device* APILearning::D3D12Context::GetDevice() const
{
	return m_Device.Get();
}

ID3D12GraphicsCommandList* APILearning::D3D12Context::GetCommandList() const
{
	return m_CommandList.Get();
}

void APILearning::D3D12Context::CreateDevice()
{
	HRESULT hr;

#if defined (_DEBUG) || defined(DEBUG)
	ID3D12Debug* m_Debugger;
	hr = D3D12GetDebugInterface(IID_PPV_ARGS(&m_Debugger));
	assert(hr == S_OK);
	m_Debugger->EnableDebugLayer();
#endif

	D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_12_0;
	hr = D3D12CreateDevice(nullptr, featureLevel, IID_PPV_ARGS(m_Device.GetAddressOf()));
	assert(hr == S_OK);


#if defined (_DEBUG) || defined(DEBUG)

	ID3D12InfoQueue* infoQueue;
	D3D12_MESSAGE_ID hide[] =
	{
		D3D12_MESSAGE_ID_MAP_INVALID_NULLRANGE,
		D3D12_MESSAGE_ID_UNMAP_INVALID_NULLRANGE,
		// Workarounds for debug layer issues on hybrid-graphics systems
		D3D12_MESSAGE_ID_EXECUTECOMMANDLISTS_WRONGSWAPCHAINBUFFERREFERENCE,
		D3D12_MESSAGE_ID_RESOURCE_BARRIER_MISMATCHING_COMMAND_LIST_TYPE,
	};
	D3D12_INFO_QUEUE_FILTER filter = {};
	filter.DenyList.NumIDs = static_cast<UINT>(sizeof(hide)/sizeof(D3D12_MESSAGE_ID));
	filter.DenyList.pIDList = hide;
	m_Device->QueryInterface(IID_PPV_ARGS(&infoQueue));
	infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true);
	infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, true);
	infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, true);
	infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_INFO, true);
	infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_MESSAGE, true);
	infoQueue->AddStorageFilterEntries(&filter);
	infoQueue->Release();
	m_Debugger->Release();

#endif
}

void APILearning::D3D12Context::CreateHeapDescriptor()
{
	HRESULT hr;
	D3D12_DESCRIPTOR_HEAP_DESC heapDesc;
	heapDesc = {};
	heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	heapDesc.NumDescriptors = m_BackBuffersAmount;
	heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	heapDesc.NodeMask = 1;
	hr = m_Device->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(m_RenderTargetViewDescHeap.GetAddressOf()));
	assert(hr == S_OK);

	size_t rtvDescriptorSize = m_Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = m_RenderTargetViewDescHeap->GetCPUDescriptorHandleForHeapStart();
	m_RenderTargetDescriptor = new D3D12_CPU_DESCRIPTOR_HANDLE[m_BackBuffersAmount];
	for (uint32_t i = 0; i < m_BackBuffersAmount; i++)
	{
		m_RenderTargetDescriptor[i] = rtvHandle;
		rtvHandle.ptr += rtvDescriptorSize;
	}

	heapDesc = {};
	heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	heapDesc.NumDescriptors = 1;
	heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	hr = m_Device->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(m_SourceDescHeap.GetAddressOf()));
	assert(hr == S_OK);
}

void APILearning::D3D12Context::CreateCommandList()
{
	HRESULT hr = m_Device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_FrameContext[0].CommandAllocator.Get(), nullptr, IID_PPV_ARGS(m_CommandList.GetAddressOf()));
	assert(hr == S_OK);
	hr = m_CommandList->Close();
	assert(hr == S_OK);
}

void APILearning::D3D12Context::CreateFence()
{
	HRESULT hr = m_Device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(m_Fence.GetAddressOf()));
	assert(hr == S_OK);

	m_FenceEvent = CreateEvent(nullptr, false, false, nullptr);
	assert(m_FenceEvent != nullptr);
}

void APILearning::D3D12Context::CreateSwapChain(HWND windowHandle)
{
	HRESULT hr;
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc = { 0 };
	swapChainDesc.BufferCount = m_BackBuffersAmount;
	swapChainDesc.Width = 0;
	swapChainDesc.Height = 0;
	swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_FRAME_LATENCY_WAITABLE_OBJECT;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
	swapChainDesc.Scaling = DXGI_SCALING_STRETCH;
	swapChainDesc.Stereo = FALSE;

	UINT dxgiFactoryFlags = 0;

#if defined (_DEBUG) || defined(DEBUG)
//#if 0
	ComPtr<IDXGIInfoQueue> dxgiInfoQueue;
	if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(dxgiInfoQueue.GetAddressOf()))))
	{
		dxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;

		dxgiInfoQueue->SetBreakOnSeverity(DXGI_DEBUG_ALL, DXGI_INFO_QUEUE_MESSAGE_SEVERITY_ERROR, true);
		dxgiInfoQueue->SetBreakOnSeverity(DXGI_DEBUG_ALL, DXGI_INFO_QUEUE_MESSAGE_SEVERITY_CORRUPTION, true);

		DXGI_INFO_QUEUE_MESSAGE_ID hide[] =
		{
			80 /* IDXGISwapChain::GetContainingOutput: The swapchain's adapter does not control the output on which the swapchain's window resides. */,
		};
		DXGI_INFO_QUEUE_FILTER filter = {};
		filter.DenyList.NumIDs = static_cast<UINT>(sizeof(hide)/sizeof(DXGI_INFO_QUEUE_MESSAGE_ID));
		filter.DenyList.pIDList = hide;
		dxgiInfoQueue->AddStorageFilterEntries(DXGI_DEBUG_DXGI, &filter);
	}
#endif

	IDXGIFactory4* dxgiFactory = nullptr;
	IDXGISwapChain1* swapChainTemp = nullptr;
	hr = CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(&dxgiFactory));
	assert(hr == S_OK);
	hr = dxgiFactory->CreateSwapChainForHwnd(m_CommandQueue.Get(), windowHandle, &swapChainDesc, nullptr, nullptr, &swapChainTemp);
	assert(hr == S_OK);
	hr = swapChainTemp->QueryInterface(IID_PPV_ARGS(m_SwapChain.GetAddressOf()));
	assert(hr == S_OK);

	swapChainTemp->Release();
	dxgiFactory->Release();
	m_SwapChain->SetMaximumFrameLatency(m_BackBuffersAmount);
	m_SwapChainWaitableObject = m_SwapChain->GetFrameLatencyWaitableObject();



}

void APILearning::D3D12Context::CreateRenderTargetView()
{
	m_RenderTargetResource = new ComPtr<ID3D12Resource>[m_BackBuffersAmount];

	for (uint32_t i = 0; i < m_BackBuffersAmount; i++)
	{
		ID3D12Resource* pBackBuffer = nullptr;
		m_SwapChain->GetBuffer(i, IID_PPV_ARGS(&pBackBuffer));
		m_Device->CreateRenderTargetView(pBackBuffer, nullptr, m_RenderTargetDescriptor[i]);
		m_RenderTargetResource[i] = pBackBuffer;
	}
}

void APILearning::D3D12Context::CreateViewport(uint32_t width, uint32_t height)
{
	m_Viewport.TopLeftX = m_Viewport.TopLeftY = m_ScissorRect.left = m_ScissorRect.top = 0;
	m_Viewport.Width = m_ScissorRect.right = (float) width;
	m_Viewport.Height = m_ScissorRect.bottom = (float) height;
	m_Viewport.MinDepth = .0f;
	m_Viewport.MaxDepth = 1.0f;

	
}

void APILearning::D3D12Context::CreateQueueDescriptor()
{
	HRESULT hr;
	D3D12_COMMAND_QUEUE_DESC queueDesc;
	queueDesc = {};
	queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	hr = m_Device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(m_CommandQueue.GetAddressOf()));
	assert(hr == S_OK);
}

void APILearning::D3D12Context::CreateFrameContext()
{
	HRESULT hr;
	m_FrameContext = new FrameContext[m_FramesInFlightAmount];
	for (uint32_t i = 0; i < m_FramesInFlightAmount; i++)
	{
		m_FrameContext[i].FenceValue = 0;
		hr = m_Device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(m_FrameContext[i].CommandAllocator.GetAddressOf()));
		assert(hr == S_OK);
	}
}

void APILearning::D3D12Context::UpdateFrameContext(uint64_t* fenceValue, FrameContext** frameContext, uint32_t* backBufferIndex, uint32_t nextFrameIndex)
{
	HANDLE waitableObjects[] = { m_SwapChainWaitableObject, nullptr };
	DWORD numWaitableObjects = 1;


	*frameContext = &m_FrameContext[nextFrameIndex % 3];
	*fenceValue = (*frameContext)->FenceValue;
	if (fenceValue != 0)
	{
		(*frameContext)->FenceValue = 0;
		m_Fence->SetEventOnCompletion(*fenceValue, m_FenceEvent);
		waitableObjects[1] = m_FenceEvent;
		numWaitableObjects = 2;
	}

	WaitForMultipleObjects(numWaitableObjects, waitableObjects, TRUE, INFINITE);

	*backBufferIndex = m_SwapChain->GetCurrentBackBufferIndex();
	(*frameContext)->CommandAllocator->Reset();
}

void APILearning::D3D12Context::UpdateCommandList(FrameContext* frameContext, uint32_t backBufferIndex)
{
	

	m_CommandList->ClearRenderTargetView(m_RenderTargetDescriptor[backBufferIndex], m_ClearColor, 0, nullptr);
	m_CommandList->OMSetRenderTargets(1, &m_RenderTargetDescriptor[backBufferIndex], FALSE, nullptr);
	m_CommandList->SetDescriptorHeaps(1, m_SourceDescHeap.GetAddressOf());
	
	
}

void APILearning::D3D12Context::UpdateFence(FrameContext** frameContext, uint64_t* fenceValue)
{
	*fenceValue = m_FenceLastSignaledValue + 1;
	m_CommandQueue->Signal(m_Fence.Get(), *fenceValue);
	m_FenceLastSignaledValue = *fenceValue;
	(*frameContext)->FenceValue = *fenceValue;
}