#include "graphics/d3d11/D3D11Context.hpp"
#include <cassert>

APILearning::D3D11Context::D3D11Context(HWND windowHandle, uint32_t width, uint32_t height)
{
    m_ClearColor[0] = 16.0f/255.0f;
    m_ClearColor[1] = 124.0f/255.0f;
    m_ClearColor[2] = 16.0f/255.0f;
    m_ClearColor[3] = 1.0f;

    CreateDevice();
    CreateSwapChain(windowHandle);
    CreateRenderTargetView();
    SetViewport(width, height);
}

APILearning::D3D11Context::~D3D11Context()
{
}

void APILearning::D3D11Context::Update()
{
    m_DeviceContext->OMSetRenderTargets(1, m_RenderTargetView.GetAddressOf(), NULL);
    m_DeviceContext->ClearRenderTargetView(m_RenderTargetView.Get(), m_ClearColor);
}

void APILearning::D3D11Context::SetClearColor(float r, float g, float b, float a)
{
    m_ClearColor[0] = r;
    m_ClearColor[1] = g;
    m_ClearColor[2] = b;
    m_ClearColor[3] = a;
}

void APILearning::D3D11Context::ReceiveCommands()
{
}

void APILearning::D3D11Context::DispatchCommands()
{
}

void APILearning::D3D11Context::NewFrame()
{
}

void APILearning::D3D11Context::EndFrame()
{
}

void APILearning::D3D11Context::Present()
{
    m_SwapChain->Present(1, 0);
}

void APILearning::D3D11Context::Draw(uint32_t elements)
{
    m_DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    m_DeviceContext->DrawIndexedInstanced(elements, 1, 0, 0, 0);
}

ID3D11Device* APILearning::D3D11Context::GetDevice() const
{
    return (ID3D11Device*)m_Device.Get();
}

ID3D11DeviceContext* APILearning::D3D11Context::GetDeviceContext() const
{
    return (ID3D11DeviceContext*)m_DeviceContext.Get();
}

void APILearning::D3D11Context::CreateDevice()
{
    D3D_FEATURE_LEVEL fl;
    UINT flags = 0;
#if defined( DEBUG ) || defined( _DEBUG )
    flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

    HRESULT hr;
    hr = D3D11CreateDevice
    (
        NULL,
        D3D_DRIVER_TYPE_HARDWARE,
        NULL,
        flags,
        NULL,
        0,
        D3D11_SDK_VERSION,
        m_Device.GetAddressOf(),
        &fl,
        m_DeviceContext.GetAddressOf()
    );

    assert(hr == S_OK);
}

void APILearning::D3D11Context::CreateSwapChain(HWND windowHandle)
{
    DXGI_SWAP_CHAIN_DESC1 swapChainDesc = { 0 };
    swapChainDesc.BufferCount = 3;
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

    IDXGIFactory4* dxgiFactory = nullptr;
    IDXGISwapChain1* swapChainTemp = nullptr;
    HRESULT hr = CreateDXGIFactory1(IID_PPV_ARGS(&dxgiFactory));
    assert(hr == S_OK);
    hr = dxgiFactory->CreateSwapChainForHwnd(m_Device.Get(), windowHandle, &swapChainDesc, nullptr, nullptr, &swapChainTemp);
    assert(hr == S_OK);
    hr = swapChainTemp->QueryInterface(IID_PPV_ARGS(m_SwapChain.GetAddressOf()));
    assert(hr == S_OK);
    swapChainTemp->Release();
    dxgiFactory->Release();
    m_SwapChain->SetMaximumFrameLatency(3);
}

void APILearning::D3D11Context::CreateRenderTargetView()
{
    ID3D11Texture2D* pBackBuffer;
    m_SwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
    HRESULT hr = m_Device->CreateRenderTargetView(pBackBuffer, NULL, m_RenderTargetView.GetAddressOf());
    assert(hr == S_OK);
    pBackBuffer->Release();
}

void APILearning::D3D11Context::SetViewport(uint32_t width, uint32_t height)
{
    m_Viewport.TopLeftX = 0;
    m_Viewport.TopLeftY = 0;
    m_Viewport.Width = (float)width;
    m_Viewport.Height = (float)height;
    m_Viewport.MinDepth = .0f;
    m_Viewport.MaxDepth = 1.0f;

    m_DeviceContext->RSSetViewports(1, &m_Viewport);
}
