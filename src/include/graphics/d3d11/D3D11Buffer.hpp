#ifndef D3D11_BUFFER_HPP
#define D3D11_BUFFER_HPP

#include "interface/Buffer.hpp"
#include "D3D11Context.hpp"

namespace APILearning
{
	class D3D11VertexBuffer : public VertexBuffer
	{

	public:
		D3D11VertexBuffer(const void* data, size_t size, uint32_t stride, const D3D11Context** context);
		~D3D11VertexBuffer();
		
		virtual void Stage() const override;
	
	private:
		ComPtr<ID3D11Buffer> m_VertexBuffer;
		ID3D11DeviceContext* m_DeviceContext;

		uint32_t m_Stride;
	};

	class D3D11IndexBuffer : public IndexBuffer
	{

	public:
		D3D11IndexBuffer(const void* data, size_t count, const D3D11Context** context);
		~D3D11IndexBuffer();

		virtual void Stage() const override;
		virtual uint32_t GetCount() const override;

	private:
		ComPtr<ID3D11Buffer> m_IndexBuffer;
		ID3D11DeviceContext* m_DeviceContext;
	};
}

#endif