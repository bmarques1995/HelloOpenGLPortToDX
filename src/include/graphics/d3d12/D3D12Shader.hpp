#ifndef D3D12_SHADER_HPP
#define D3D12_SHADER_HPP

#include "interface/Shader.hpp"
#include "utils/ShaderManager.hpp"
#include "D3D12Context.hpp"

#include <d3d12.h>

#include <wrl.h>

using Microsoft::WRL::ComPtr;

namespace APILearning
{
	class D3D12Shader : public Shader
	{
	public:
		D3D12Shader(const D3D12Context** context, const ShaderGroup& shaderGroup, const std::initializer_list<BufferElement>& elements);
		~D3D12Shader();

		virtual void Stage() const override;
		virtual uint32_t GetStride() const override;
		virtual uint32_t GetOffset() const override;
	private:
		ComPtr<ID3D12PipelineState> m_GraphicsPipeline;
		ComPtr<ID3D12RootSignature> m_RootSignature;
		//ComPtr<ID3D12PipelineState> m_ComputePipeline;

		ComPtr<ID3DBlob> m_VertexBlob;
		ComPtr<ID3DBlob> m_PixelBlob;
		ComPtr<ID3DBlob> m_GeometryBlob;
		ComPtr<ID3DBlob> m_HullBlob;
		ComPtr<ID3DBlob> m_DomainBlob;
		ComPtr<ID3DBlob> m_ComputeBlob;

		ID3D12GraphicsCommandList* m_CommandList;
		BufferLayout m_Layout;
		ShaderGroup m_ShaderGroup;

		DXGI_FORMAT GetFormat(ShaderDataType type);

		void PushShader(std::string_view baseShaderPath, SHADER_KIND shaderKind, HLSL_VERSION hlslVersion, D3D12_GRAPHICS_PIPELINE_STATE_DESC* graphicsDesc);
		void PushShader(std::string_view baseShaderPath, SHADER_KIND shaderKind, HLSL_VERSION hlslVersion, D3D12_COMPUTE_PIPELINE_STATE_DESC* computeDesc);

		void BuildBlender(D3D12_GRAPHICS_PIPELINE_STATE_DESC* graphicsDesc);
		void BuildRasterizer(D3D12_GRAPHICS_PIPELINE_STATE_DESC* graphicsDesc);
		void BuildDepthStencil(D3D12_GRAPHICS_PIPELINE_STATE_DESC* graphicsDesc);

		void CreateGraphicsRootSignature(ID3D12RootSignature** rootSignature, ID3D12Device* device);
		void CreateComputeRootSignature(ID3D12RootSignature** rootSignature, ID3D12Device* device);
	};
}

#endif // D3D12_SHADER_HPP
