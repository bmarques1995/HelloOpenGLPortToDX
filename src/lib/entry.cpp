#include <sstream>
#include <windows.h>
#include <d3d11_4.h>

int program();

INT APIENTRY wWinMain(_In_ HINSTANCE hInstance, 
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR lpCmdLine,
    _In_ int nCmdShow)
{
    return program();
}

int program()
{
    //window
    HINSTANCE hInstance = GetModuleHandleW(NULL);
    LPWSTR cmdLine = GetCommandLineW();
    DWORD cmdShow = 10;
    RECT windowDimensions = {0, 0, 640, 480};

    WNDCLASSEXW windowClass = {0};
    windowClass.hInstance = hInstance;
    windowClass.cbSize = sizeof(windowClass);
    windowClass.cbClsExtra = 0;
    windowClass.cbWndExtra = sizeof(void*);
    windowClass.style = CS_OWNDC | CS_VREDRAW | CS_HREDRAW;
    windowClass.hIcon = nullptr;
    windowClass.hCursor = LoadCursor(hInstance, IDC_ARROW);
    windowClass.hbrBackground = CreateSolidBrush(RGB(0,0,0));
    windowClass.lpszClassName = L"Hello DirectX";
    windowClass.lpszMenuName = nullptr;
    
    windowClass.lpfnWndProc = [](HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) -> LRESULT
    {
        switch (msg)
        {
        case WM_CLOSE:
        case WM_QUIT:
            PostQuitMessage(69);
            return 65;
        default:
            return DefWindowProcW(hWnd, msg, wParam, lParam);
        }
    };

    DWORD windowedFlags = WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_SYSMENU | WS_MINIMIZEBOX | WS_CAPTION | WS_MAXIMIZEBOX | WS_THICKFRAME;
    
    AdjustWindowRectEx(&windowDimensions, windowedFlags, 0, 0);

    RegisterClassExW(&windowClass);

    HWND window = CreateWindowExW
    (
        0,
        L"Hello DirectX",
        L"Hello World",
        windowedFlags,
        100,
        100,
        windowDimensions.right - windowDimensions.left,
        windowDimensions.bottom - windowDimensions.top,
        nullptr,
        nullptr,
        windowClass.hInstance,
        nullptr
    );

    if (GetLastError() != 0)
        return -1;

    ShowWindow(window, cmdShow);
    UpdateWindow(window);

    MSG msg = {0};
    bool shouldStop = false;

    //Graphics

    ID3D11Device* device;
    ID3D11DeviceContext* deviceContext;
    IDXGISwapChain* swapChain;
    ID3D11RenderTargetView* renderTargetView;

    DXGI_SWAP_CHAIN_DESC swapChainDesc = {0};
    swapChainDesc.BufferDesc.RefreshRate.Numerator = 0;
    swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
    swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    swapChainDesc.SampleDesc.Count = 1;
    swapChainDesc.SampleDesc.Quality = 0;
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDesc.BufferCount = 3;
    swapChainDesc.OutputWindow = window;
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
        &swapChain,
        &device,
        &fl,
        &deviceContext
    );

    if (hr != S_OK)
        return -2;

    ID3D11Texture2D* pBackBuffer;
    swapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
    hr = device->CreateRenderTargetView(pBackBuffer, NULL, &renderTargetView);
    if (hr != S_OK)
        return -3;
    pBackBuffer->Release();

    float color[4] = {.7f, .3f,.9f, 1.0f};

    while (!shouldStop)
    {
        //window loop
        if (::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            ::TranslateMessage(&msg);
            ::DispatchMessage(&msg);
            if ((msg.message == WM_QUIT)||(msg.message == WM_CLOSE))
                shouldStop = true;
        }
        //graphics loop
        deviceContext->OMSetRenderTargets(1, &renderTargetView, NULL);
        deviceContext->ClearRenderTargetView(renderTargetView, color);
        swapChain->Present(1, 0);
    }
    return 0;
}
