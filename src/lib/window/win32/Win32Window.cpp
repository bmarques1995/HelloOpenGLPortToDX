#include "window/win32/Win32Window.hpp"
#include <cassert>

APILearning::Win32Window::Win32Window(WindowProps props)
{
    m_Title = std::wstring(props.Title.begin(), props.Title.end());
    m_Width = props.Width;
    m_Height = props.Height;
    m_ShouldClose = false;
    //window
    HINSTANCE hInstance = GetModuleHandleW(NULL);
    LPWSTR cmdLine = GetCommandLineW();
    DWORD cmdShow = 10;
    RECT windowDimensions = { 0, 0, m_Width, m_Height };

    CreateWindowClass(&hInstance);
    RegisterClassExW(&m_WindowClass);

    DWORD windowedFlags = WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_SYSMENU | WS_MINIMIZEBOX | WS_CAPTION | WS_MAXIMIZEBOX | WS_THICKFRAME;

    AdjustDimensions(&windowDimensions, windowedFlags);

    m_WindowHandle = CreateWindowExW
    (
        0,
        m_Title.c_str(),
        m_Title.c_str(),
        windowedFlags,
        100,
        100,
        windowDimensions.right - windowDimensions.left,
        windowDimensions.bottom - windowDimensions.top,
        nullptr,
        nullptr,
        m_WindowClass.hInstance,
        nullptr
    );

    auto test = GetLastError();
    assert(GetLastError() == 0);

    ShowWindow(m_WindowHandle, cmdShow);
    UpdateWindow(m_WindowHandle);

    
}

APILearning::Win32Window::~Win32Window()
{
    DestroyWindow(m_WindowHandle);
    UnregisterClassW(m_WindowClass.lpszClassName, m_WindowClass.hInstance);
}

void APILearning::Win32Window::Update()
{
    if (::PeekMessage(&m_MSG, NULL, 0, 0, PM_REMOVE))
    {
        ::TranslateMessage(&m_MSG);
        ::DispatchMessage(&m_MSG);
        if ((m_MSG.message == WM_QUIT) || (m_MSG.message == WM_CLOSE))
            m_ShouldClose = true;
    }
}

uint32_t APILearning::Win32Window::GetWidth()
{
	return m_Width;
}

uint32_t APILearning::Win32Window::GetHeight()
{
	return m_Height;
}

bool APILearning::Win32Window::ShouldClose()
{
	return m_ShouldClose;
}

std::any APILearning::Win32Window::GetNativeWindow()
{
    return m_WindowHandle;
}

void APILearning::Win32Window::CreateWindowClass(HINSTANCE* instance)
{
    m_WindowClass = { 0 };
    m_WindowClass.hInstance = *instance;
    m_WindowClass.cbSize = sizeof(m_WindowClass);
    m_WindowClass.cbClsExtra = 0;
    m_WindowClass.cbWndExtra = sizeof(void*);
    m_WindowClass.style = CS_OWNDC | CS_VREDRAW | CS_HREDRAW;
    m_WindowClass.hIcon = nullptr;
    m_WindowClass.hCursor = LoadCursor(m_WindowClass.hInstance, IDC_ARROW);
    m_WindowClass.hbrBackground = CreateSolidBrush(RGB(0, 0, 0));
    m_WindowClass.lpszClassName = m_Title.c_str();
    m_WindowClass.lpszMenuName = nullptr;

    m_WindowClass.lpfnWndProc = [](HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) -> LRESULT
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
}

void APILearning::Win32Window::AdjustDimensions(LPRECT originalDimensions, DWORD flags)
{
    AdjustWindowRectEx(originalDimensions, flags, 0, 0);
}
