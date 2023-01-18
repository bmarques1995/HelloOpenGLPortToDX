#include "graphics/d3d9/D3D9Context.hpp"]
#include <cassert>

APILearning::D3D9Context::D3D9Context(HWND windowHandle)
{
    m_ClearColor[0] = 116.0f/225.0f;
    m_ClearColor[1] = 77.0f/255.0f;
    m_ClearColor[2] = 169.0f/255.0f;
    m_ClearColor[3] = 1.0f;

    UINT flags = D3D_SDK_VERSION;

#if defined( DEBUG ) || defined( _DEBUG )
    flags |= 0x80000000;
#endif

    m_Direct3D9API = Direct3DCreate9(flags);
    assert(m_Direct3D9API != nullptr);

    m_PresentInfo.Windowed = TRUE;
    m_PresentInfo.SwapEffect = D3DSWAPEFFECT_FLIP;
    m_PresentInfo.BackBufferFormat = D3DFMT_UNKNOWN;
    m_PresentInfo.EnableAutoDepthStencil = TRUE;
    m_PresentInfo.BackBufferCount = 3;
    m_PresentInfo.AutoDepthStencilFormat = D3DFMT_D24S8;
    m_PresentInfo.PresentationInterval = D3DPRESENT_INTERVAL_ONE;

    assert(m_Direct3D9API->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, windowHandle, D3DCREATE_HARDWARE_VERTEXPROCESSING, &m_PresentInfo, m_Device.GetAddressOf()) >= 0);
}

APILearning::D3D9Context::~D3D9Context()
{
    m_Device->Release();
    m_Direct3D9API->Release();
}

void APILearning::D3D9Context::Update()
{
    m_Device->SetRenderState(D3DRS_ZENABLE, TRUE);
    m_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
    m_Device->SetRenderState(D3DRS_SCISSORTESTENABLE, FALSE);
    D3DCOLOR clear_color = D3DCOLOR_RGBA( (unsigned)(m_ClearColor[0] * 255.0f), (unsigned)(m_ClearColor[1] * 255.0f), (unsigned)(m_ClearColor[2] * 255.0f), (unsigned)(m_ClearColor[3] * 255.0f) );
    m_Device->Clear(0, nullptr, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, clear_color, 1.0f, 0);
    
}

void APILearning::D3D9Context::SetClearColor(float r, float g, float b, float a)
{
    m_ClearColor[0] = r;
    m_ClearColor[1] = g;
    m_ClearColor[2] = b;
    m_ClearColor[3] = a;
}

void APILearning::D3D9Context::ReceiveCommands()
{
}

void APILearning::D3D9Context::DispatchCommands()
{
}

void APILearning::D3D9Context::NewFrame()
{
}

void APILearning::D3D9Context::EndFrame()
{
}

void APILearning::D3D9Context::Present()
{
    m_Device->Present(nullptr, nullptr, nullptr, nullptr);
}
