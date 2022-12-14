#include <sstream>
#include <windows.h>

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
        L"Hello DirectX",
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

    auto x = GetLastError();

    ShowWindow(window, cmdShow);
    UpdateWindow(window);

    MSG msg = {0};
    bool shouldStop = false;

    //Graphics

    while (!shouldStop)
    {
        if (::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            ::TranslateMessage(&msg);
            ::DispatchMessage(&msg);
            if ((msg.message == WM_QUIT)||(msg.message == WM_CLOSE))
                shouldStop = true;
        }
    }
    return 0;
}
