#include "graphics/d3d11/D3D11Context.hpp"
#include <cassert>

APILearning::D3D11Context::D3D11Context(HWND windowHandle)
{
    m_ClearColor[0] = .7f;
    m_ClearColor[1] = .3f;
    m_ClearColor[2] = .9f;
    m_ClearColor[3] = 1.0f;

    DXGI_SWAP_CHAIN_DESC swapChainDesc = { 0 };
    swapChainDesc.BufferDesc.RefreshRate.Numerator = 0;
    swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
    swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    swapChainDesc.SampleDesc.Count = 1;
    swapChainDesc.SampleDesc.Quality = 0;
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDesc.BufferCount = 3;
    swapChainDesc.OutputWindow = windowHandle;
    swapChainDesc.Windowed = true;
    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;

    D3D_FEATURE_LEVEL fl;
    UINT flags = 0;
#if defined( DEBUG ) || defined( _DEBUG )
    flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

    HRESULT hr;
    hr = D3D11CreateDeviceAndSwapChain
    (
        NULL,
        D3D_DRIVER_TYPE_HARDWARE,
        NULL,
        flags,
        NULL,
        0,
        D3D11_SDK_VERSION,
        &swapChainDesc,
        m_SwapChain.GetAddressOf(),
        m_Device.GetAddressOf(),
        &fl,
        m_DeviceContext.GetAddressOf()
    );

    assert(hr == S_OK);

    ID3D11Texture2D* pBackBuffer;
    m_SwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
    hr = m_Device->CreateRenderTargetView(pBackBuffer, NULL, m_RenderTargetView.GetAddressOf());
    assert(hr == S_OK);
    pBackBuffer->Release();
}

APILearning::D3D11Context::~D3D11Context()
{
}

void APILearning::D3D11Context::Update()
{
    m_DeviceContext->OMSetRenderTargets(1, m_RenderTargetView.GetAddressOf(), NULL);
    m_DeviceContext->ClearRenderTargetView(m_RenderTargetView.Get(), m_ClearColor);
    m_SwapChain->Present(1, 0);
}

void APILearning::D3D11Context::SetClearColor(float r, float g, float b, float a)
{
    m_ClearColor[0] = r;
    m_ClearColor[1] = g;
    m_ClearColor[2] = b;
    m_ClearColor[3] = a;
}
