#ifndef D3D12_BUFFER_HPP
#define D3D12_BUFFER_HPP

#include "interface/Buffer.hpp"
#include "D3D12Context.hpp"

namespace APILearning
{
	class D3D12Buffer
	{
	protected:
		void CreateBuffer(const void* data, size_t size, const D3D12Context** context);

		ComPtr<ID3D12Resource> m_Buffer;
	};

	class D3D12VertexBuffer : public VertexBuffer, public D3D12Buffer
	{

	public:
		D3D12VertexBuffer(const void* data, size_t size, uint32_t stride, const D3D12Context** context);
		~D3D12VertexBuffer();

		virtual void Stage() const override;

	private:
		
		ID3D12GraphicsCommandList* m_CommandList;
		D3D12_VERTEX_BUFFER_VIEW m_VertexBufferView;

		uint32_t m_Stride;
	};

	class D3D12IndexBuffer : public IndexBuffer, public D3D12Buffer
	{

	public:
		D3D12IndexBuffer(const void* data, size_t count, const D3D12Context** context);
		~D3D12IndexBuffer();

		virtual void Stage() const override;
		virtual uint32_t GetCount() const override;

	private:
		ID3D12GraphicsCommandList* m_CommandList;
		D3D12_INDEX_BUFFER_VIEW m_IndexBufferView;
	};
}

#endif // D3D12_BUFFER_HPP
