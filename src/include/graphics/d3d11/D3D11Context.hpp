#ifndef D3D11_CONTEXT_HPP
#define D3D11_CONTEXT_HPP

#include "interface/GraphicsContext.hpp"
#include <d3d11_4.h>

namespace APILearning
{
	class D3D11Context : public GraphicsContext
	{
	public:
		D3D11Context(HWND windowHandle);
		~D3D11Context();

		virtual void Update() override;
		virtual void SetClearColor(float r, float g, float b, float a) override;

	private:
		ID3D11Device* m_Device;
		ID3D11DeviceContext* m_DeviceContext;
		IDXGISwapChain* m_SwapChain;
		ID3D11RenderTargetView* m_RenderTargetView;

		float m_ClearColor[4];
	};
}

#endif // D3D11_CONTEXT_HPP
