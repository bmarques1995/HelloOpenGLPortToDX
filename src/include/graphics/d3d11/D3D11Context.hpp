#ifndef D3D11_CONTEXT_HPP
#define D3D11_CONTEXT_HPP

#include "interface/GraphicsContext.hpp"
#include <d3d11_4.h>

#include <wrl.h>

using Microsoft::WRL::ComPtr;

namespace APILearning
{
	class D3D11Context : public GraphicsContext
	{
	public:
		D3D11Context(HWND windowHandle, uint32_t width, uint32_t height);
		~D3D11Context();

		virtual void Update() override;
		virtual void SetClearColor(float r, float g, float b, float a) override;

		virtual void ReceiveCommands() override;
		virtual void DispatchCommands() override;
		virtual void NewFrame() override;
		virtual void EndFrame() override;
		virtual void Present() override;
		virtual void Draw(uint32_t elements) override;

		ID3D11Device* GetDevice() const;
		ID3D11DeviceContext* GetDeviceContext() const;
	private:
		ComPtr<ID3D11Device> m_Device;
		ComPtr<ID3D11DeviceContext> m_DeviceContext;
		ComPtr<IDXGISwapChain3> m_SwapChain;
		ComPtr<ID3D11RenderTargetView> m_RenderTargetView;

		D3D11_VIEWPORT m_Viewport;

		void CreateDevice();
		void CreateSwapChain(HWND windowHandle);
		void CreateRenderTargetView();
		void SetViewport(uint32_t width, uint32_t height);

		float m_ClearColor[4];
	};
}

#endif // D3D11_CONTEXT_HPP
