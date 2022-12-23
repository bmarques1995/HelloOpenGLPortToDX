#include <sstream>
#include <windows.h>

#include "interface/Window.hpp"
#include "interface/GraphicsContext.hpp"
#include "window/win32/Win32Window.hpp"
#include "graphics/d3d11/D3D11Context.hpp"
#include "graphics/d3d12/D3D12Context.hpp"

#include <d3d12.h>

struct FrameContext
{
    ID3D12CommandAllocator* CommandAllocator;
    UINT64                  FenceValue;
};

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
    APILearning::GraphicsContext* context = new APILearning::D3D11Context(std::any_cast<HWND>(window->GetNativeWindow()));
    context->SetClearColor(.7f,.3f,.9f,1.0f);

    while (!window->ShouldClose())
    {
        window->Update();
        context->Update();
    }

    delete context;
    delete window;
    return 0;
}
