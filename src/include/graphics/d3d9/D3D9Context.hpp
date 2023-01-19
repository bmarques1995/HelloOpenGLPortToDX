#ifndef D3D9_CONTEXT_HPP
#define D3D9_CONTEXT_HPP

#include "interface/GraphicsContext.hpp"

#include <d3d9.h>

#include <wrl.h>

using Microsoft::WRL::ComPtr;

namespace APILearning
{
	class D3D9Context : public GraphicsContext
	{
	public:
		D3D9Context(HWND windowHandle);
		~D3D9Context();

		virtual void Update() override;
		virtual void SetClearColor(float r, float g, float b, float a) override;

		virtual void ReceiveCommands() override;
		virtual void DispatchCommands() override;
		virtual void NewFrame() override;
		virtual void EndFrame() override;
		virtual void Present() override;

		virtual void Draw(uint32_t elements) override;

	private:
		ComPtr<IDirect3D9> m_Direct3D9API;
		ComPtr<IDirect3DDevice9> m_Device;
		D3DPRESENT_PARAMETERS m_PresentInfo = {};

		float m_ClearColor[4];
	};
}

#endif // D3D11_CONTEXT_HPP
