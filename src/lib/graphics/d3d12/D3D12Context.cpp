#include "graphics/d3d12/D3D12Context.hpp"
#include <cassert>

APILearning::D3D12Context::D3D12Context(HWND windowHandle, uint32_t numBackBuffers, uint32_t numFramesInFlight) :
	m_BackBuffersAmount(numBackBuffers), m_FramesInFlightAmount(numFramesInFlight)
{
	m_ClearColor[0] = .7f;
	m_ClearColor[1] = .3f;
	m_ClearColor[2] = .9f;
	m_ClearColor[3] = 1.0f;
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

#if defined (_DEBUG) || defined(DEBUG)
	ID3D12Debug* m_Debugger;
	hr = D3D12GetDebugInterface(IID_PPV_ARGS(&m_Debugger));
	assert(hr == S_OK);
	m_Debugger->EnableDebugLayer();
#endif

	D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_12_0;
	hr = D3D12CreateDevice(nullptr, featureLevel, IID_PPV_ARGS(&m_Device));
	assert(hr == S_OK);

#if defined (_DEBUG) || defined(DEBUG)
	ID3D12InfoQueue* infoQueue;
	m_Device->QueryInterface(IID_PPV_ARGS(&infoQueue));
	infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true);
	infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, true);
	infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, true);
	infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_INFO, true);
	infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_MESSAGE, true);
	infoQueue->Release();
	m_Debugger->Release();
#endif

	D3D12_DESCRIPTOR_HEAP_DESC heapDesc;
	heapDesc = {};
	heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	heapDesc.NumDescriptors = m_BackBuffersAmount;
	heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	heapDesc.NodeMask = 1;
	hr = m_Device->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&m_RenderTargetViewDescHeap));
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
	hr = m_Device->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&m_SourceDescHeap));
	assert(hr == S_OK);

	D3D12_COMMAND_QUEUE_DESC queueDesc;
	queueDesc = {};
	queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	hr = m_Device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&m_CommandQueue));
	assert(hr == S_OK);

	m_FrameContext = new FrameContext[m_FramesInFlightAmount];
	for (uint32_t i = 0; i < m_FramesInFlightAmount; i++)
	{
		m_FrameContext[i].FenceValue = 0;
		hr = m_Device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_FrameContext[i].CommandAllocator));
		assert(hr == S_OK);
	}

	hr = m_Device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_FrameContext[0].CommandAllocator, nullptr, IID_PPV_ARGS(&m_CommandList));
	assert(hr == S_OK);
	hr = m_CommandList->Close();
	assert(hr == S_OK);

	hr = m_Device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_Fence));
	assert(hr == S_OK);

	m_FenceEvent = CreateEvent(nullptr, false, false, nullptr);
	assert(m_FenceEvent != nullptr);

	IDXGIFactory4* dxgiFactory = nullptr;
	IDXGISwapChain1* swapChainTemp = nullptr;
	hr = CreateDXGIFactory1(IID_PPV_ARGS(&dxgiFactory));
	assert(hr == S_OK);
	hr = dxgiFactory->CreateSwapChainForHwnd(m_CommandQueue, windowHandle, &swapChainDesc, nullptr, nullptr, &swapChainTemp);
	assert(hr == S_OK);
	hr = swapChainTemp->QueryInterface(IID_PPV_ARGS(&m_SwapChain));
	assert(hr == S_OK);
	swapChainTemp->Release();
	dxgiFactory->Release();
	m_SwapChain->SetMaximumFrameLatency(m_BackBuffersAmount);
	m_SwapChainWaitableObject = m_SwapChain->GetFrameLatencyWaitableObject();

	m_RenderTargetResource = new ID3D12Resource * [m_BackBuffersAmount];

	for (uint32_t i = 0; i < m_BackBuffersAmount; i++)
	{
		ID3D12Resource* pBackBuffer = NULL;
		m_SwapChain->GetBuffer(i, IID_PPV_ARGS(&pBackBuffer));
		m_Device->CreateRenderTargetView(pBackBuffer, NULL, m_RenderTargetDescriptor[i]);
		m_RenderTargetResource[i] = pBackBuffer;
	}
}

APILearning::D3D12Context::~D3D12Context()
{
	delete[] m_RenderTargetResource;
	delete[] m_FrameContext;
	delete[] m_RenderTargetDescriptor;
}

void APILearning::D3D12Context::Update()
{
	UINT nextFrameIndex = m_FrameIndex + 1;
	m_FrameIndex = nextFrameIndex;

	HANDLE waitableObjects[] = { m_SwapChainWaitableObject, nullptr };
	DWORD numWaitableObjects = 1;

	FrameContext* frameContext = &m_FrameContext[nextFrameIndex % 3];
	uint64_t fenceValue = frameContext->FenceValue;
	if (fenceValue != 0)
	{
		frameContext->FenceValue = 0;
		m_Fence->SetEventOnCompletion(fenceValue, m_FenceEvent);
		waitableObjects[1] = m_FenceEvent;
		numWaitableObjects = 2;
	}

	WaitForMultipleObjects(numWaitableObjects, waitableObjects, TRUE, INFINITE);

	uint32_t backBufferIndex = m_SwapChain->GetCurrentBackBufferIndex();
	frameContext->CommandAllocator->Reset();

	D3D12_RESOURCE_BARRIER barrier = {};
	barrier.Type - D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.pResource = m_RenderTargetResource[backBufferIndex];
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
	m_CommandList->Reset(frameContext->CommandAllocator, NULL);
	m_CommandList->ResourceBarrier(1, &barrier);

	m_CommandList->ClearRenderTargetView(m_RenderTargetDescriptor[backBufferIndex], m_ClearColor, 0, NULL);
	m_CommandList->OMSetRenderTargets(1, &m_RenderTargetDescriptor[backBufferIndex], FALSE, NULL);
	m_CommandList->SetDescriptorHeaps(1, &m_SourceDescHeap);
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
	m_CommandList->ResourceBarrier(1, &barrier);
	m_CommandList->Close();

	m_CommandQueue->ExecuteCommandLists(1, (ID3D12CommandList* const*)&m_CommandList);

	m_SwapChain->Present(1, 0);

	fenceValue = m_FenceLastSignaledValue + 1;
	m_CommandQueue->Signal(m_Fence, fenceValue);
	m_FenceLastSignaledValue = fenceValue;
	frameContext->FenceValue = fenceValue;
}

void APILearning::D3D12Context::SetClearColor(float r, float g, float b, float a)
{
	m_ClearColor[0] = r;
	m_ClearColor[1] = g;
	m_ClearColor[2] = b;
	m_ClearColor[3] = a;
}
