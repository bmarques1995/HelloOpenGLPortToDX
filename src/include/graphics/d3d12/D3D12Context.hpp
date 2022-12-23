#ifndef D3D12_CONTEXT_HPP
#define D3D12_CONTEXT_HPP

#include "interface/GraphicsContext.hpp"
#include <d3d12.h>
#include <dxgi1_6.h>
#include <cstdint>

#include <wrl.h>

using Microsoft::WRL::ComPtr;

namespace APILearning
{
	struct FrameContext
	{
		ComPtr<ID3D12CommandAllocator> CommandAllocator;
		uint64_t FenceValue;
	};

	class D3D12Context : public GraphicsContext
	{
	public:
		D3D12Context(HWND windowHandle, uint32_t numBackBuffers = 3, uint32_t numFramesInFlight = 3);
		~D3D12Context();

		virtual void Update() override;
		virtual void SetClearColor(float r, float g, float b, float a) override;

	private:
		float m_ClearColor[4];

		ComPtr<IDXGISwapChain3> m_SwapChain;
		HANDLE m_SwapChainWaitableObject;
		ComPtr<ID3D12Device> m_Device;
		ComPtr<ID3D12DescriptorHeap> m_RenderTargetViewDescHeap;
		ComPtr<ID3D12Resource>* m_RenderTargetResource;
		ComPtr<ID3D12DescriptorHeap> m_SourceDescHeap;
		ComPtr<ID3D12CommandQueue> m_CommandQueue;
		ComPtr<ID3D12GraphicsCommandList> m_CommandList;
		ComPtr<ID3D12Fence> m_Fence;
		HANDLE m_FenceEvent;
		D3D12_CPU_DESCRIPTOR_HANDLE* m_RenderTargetDescriptor;
		FrameContext* m_FrameContext;

		uint32_t m_FrameIndex = 0;
		uint64_t m_FenceLastSignaledValue = 0;

		uint32_t m_BackBuffersAmount;
		uint32_t m_FramesInFlightAmount;
	};
}

#endif // D3D12_CONTEXT_HPP
