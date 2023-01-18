#include <sstream>
#include <windows.h>

#include "interface/Window.hpp"
#include "interface/GraphicsContext.hpp"
#include "window/win32/Win32Window.hpp"
#include "graphics/d3d11/D3D11Context.hpp"
#include "graphics/d3d12/D3D12Context.hpp"
#include "graphics/d3d9/D3D9Context.hpp"

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
    APILearning::Window* window = new APILearning::Win32Window();
    APILearning::GraphicsContext* context = new APILearning::D3D12Context(std::any_cast<HWND>(window->GetNativeWindow()));

    while (!window->ShouldClose())
    {
        window->Update();
        context->NewFrame();
        context->ReceiveCommands();
        context->Update();
        context->DispatchCommands();
        context->Present();
        context->EndFrame();
    }

    delete context;
    delete window;
    return 0;
}
