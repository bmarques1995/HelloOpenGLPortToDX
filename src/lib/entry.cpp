
#include <sstream>
#include <windows.h>

#include "interface/Window.hpp"
#include "interface/GraphicsContext.hpp"
#include "interface/Shader.hpp"
#include "window/win32/Win32Window.hpp"
#include "graphics/d3d11/D3D11Context.hpp"
#include "graphics/d3d11/D3D11Shader.hpp"
#include "graphics/d3d11/D3D11Buffer.hpp"
#include "graphics/d3d12/D3D12Context.hpp"
#include "graphics/d3d12/D3D12Shader.hpp"
#include "graphics/d3d12/D3D12Buffer.hpp"
#include "graphics/d3d9/D3D9Context.hpp"

#include "utils/ShaderManager.hpp"

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
	APILearning::ShaderGroup shaderGroup("assets/shaders/HLSL/triangle", APILearning::HLSL_VERSION::HLSL_5_0,
		{
			{APILearning::SHADER_KIND::D3D_SHADER_KIND_VERTEX},
			{APILearning::SHADER_KIND::D3D_SHADER_KIND_PIXEL}
		}
	);
	APILearning::ShaderManager::CompileShaderFamily(shaderGroup);
	std::initializer_list<APILearning::BufferElement> bufferElements = {
			{APILearning::ShaderDataType::Float3, "POSITION", false},
			{APILearning::ShaderDataType::Float4, "COLOR", false}
	};

	float vBuffer[] =
	{
		.0f, .5f, .0f, 1.0f, .0f, .0f, 1.0f,
		.5f, -.5f, .0f, .0f, 1.0f, .0f, 1.0f,
		-.5f, -.5f, .0f, .0f, .0f, 1.0f, 1.0f
	};

	uint32_t iBuffer[] =
	{
		0,1,2
	};

	APILearning::Window* window = new APILearning::Win32Window();
	APILearning::GraphicsContext* context = new APILearning::D3D12Context(std::any_cast<HWND>(window->GetNativeWindow()), window->GetWidth(), window->GetHeight());
	APILearning::Shader* shader = new APILearning::D3D12Shader((const APILearning::D3D12Context**)(&context), shaderGroup, bufferElements);
	APILearning::VertexBuffer* vertexBuffer = new APILearning::D3D12VertexBuffer((const void*)vBuffer, sizeof(vBuffer), shader->GetStride(), (const APILearning::D3D12Context**)(&context));
	APILearning::IndexBuffer* indexBuffer = new APILearning::D3D12IndexBuffer((const void*)iBuffer, sizeof(iBuffer) / sizeof(uint32_t), (const APILearning::D3D12Context**)(&context));

	while (!window->ShouldClose())
	{
		window->Update();
		context->NewFrame();
		context->ReceiveCommands();
		context->Update();
		shader->Stage();
		vertexBuffer->Stage();
		indexBuffer->Stage();
		context->Draw(indexBuffer->GetCount());
		context->DispatchCommands();
		context->Present();
		context->EndFrame();
	}

	//delete indexBuffer;
	//delete vertexBuffer;
	delete shader;
	delete context;
	delete window;
	return 0;
}
