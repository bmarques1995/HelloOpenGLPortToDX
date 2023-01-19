#include "graphics/d3d11/D3D11Buffer.hpp"

APILearning::D3D11VertexBuffer::D3D11VertexBuffer(const void* data, size_t size, uint32_t stride, const D3D11Context** context)
{
	m_DeviceContext = (*context)->GetDeviceContext();
	m_Stride = stride;

	HRESULT hr;

	D3D11_BUFFER_DESC bd;
	D3D11_SUBRESOURCE_DATA initData;

	ZeroMemory(&bd, sizeof(bd));
	ZeroMemory(&initData, sizeof(initData));

	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = (UINT)(size);
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;

	initData.pSysMem = data;
	initData.SysMemPitch = 0;
	initData.SysMemSlicePitch = 0;

	ID3D11Device* device = (*context)->GetDevice();

	hr = device->CreateBuffer(&bd, &initData, m_VertexBuffer.GetAddressOf());
}

APILearning::D3D11VertexBuffer::~D3D11VertexBuffer()
{
	m_DeviceContext = nullptr;
}

void APILearning::D3D11VertexBuffer::Stage() const
{
	UINT offset = 0;
	m_DeviceContext->IASetVertexBuffers(0, 1, m_VertexBuffer.GetAddressOf(), &m_Stride, &offset);
}

APILearning::D3D11IndexBuffer::D3D11IndexBuffer(const void* data, size_t count, const D3D11Context** context)
{
	m_Count = (uint32_t) count;
	m_DeviceContext = (*context)->GetDeviceContext();

	HRESULT hr;

	D3D11_BUFFER_DESC bd;
	D3D11_SUBRESOURCE_DATA initData;

	ZeroMemory(&bd, sizeof(bd));
	ZeroMemory(&initData, sizeof(initData));

	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = (UINT)(count * sizeof(uint32_t));
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = 0;

	initData.pSysMem = data;
	initData.SysMemPitch = 0;
	initData.SysMemSlicePitch = 0;

	ID3D11Device* device = (*context)->GetDevice();

	hr = device->CreateBuffer(&bd, &initData, m_IndexBuffer.GetAddressOf());
}

APILearning::D3D11IndexBuffer::~D3D11IndexBuffer()
{
	m_DeviceContext = nullptr;
}

void APILearning::D3D11IndexBuffer::Stage() const
{
	m_DeviceContext->IASetIndexBuffer(m_IndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
}

uint32_t APILearning::D3D11IndexBuffer::GetCount() const
{
	return m_Count;
}
