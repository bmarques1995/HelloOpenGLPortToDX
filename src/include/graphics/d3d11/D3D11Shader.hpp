#ifndef D3D11_SHADER_HPP
#define D3D11_SHADER_HPP

#include "interface/Shader.hpp"
#include "D3D11Context.hpp"
#include "utils/ShaderManager.hpp"
#include <functional>
#include <unordered_map>
#include <any>

namespace APILearning
{
	class D3D11Shader : public Shader
	{
	public:
		D3D11Shader(const D3D11Context** context, const ShaderGroup& shaderGroup, const std::initializer_list<BufferElement>& elements);
		~D3D11Shader();

		virtual void Stage() const override;

		virtual uint32_t GetStride() const override;
		virtual uint32_t GetOffset() const override;
	private:

		ComPtr<ID3D11InputLayout> m_InputLayout;
		
		ComPtr<ID3D11VertexShader> m_VertexShader;
		ComPtr<ID3DBlob> m_VertexBlob;
		ComPtr<ID3D11PixelShader> m_PixelShader;
		ComPtr<ID3DBlob> m_PixelBlob;
		ComPtr<ID3D11GeometryShader> m_GeometryShader;
		ComPtr<ID3DBlob> m_GeometryBlob;
		ComPtr<ID3D11HullShader> m_HullShader;
		ComPtr<ID3DBlob> m_HullBlob;
		ComPtr<ID3D11DomainShader> m_DomainShader;
		ComPtr<ID3DBlob> m_DomainBlob;
		ComPtr<ID3D11ComputeShader> m_ComputeShader;
		ComPtr<ID3DBlob> m_ComputeBlob;

		std::unordered_map<SHADER_KIND, std::function<void(std::string_view, ID3D11Device*)>> m_ShaderBuilders;
		mutable std::unordered_map<SHADER_KIND, std::function<void(std::any*, const ID3D11DeviceContext*)>> m_ShaderStagers;
		mutable std::unordered_map<SHADER_KIND, std::any> m_ShaderPointers;

		//temp will be replaced with lambdas
		void BuildShader(std::string_view baseShaderPath, SHADER_KIND shaderKind, HLSL_VERSION hlslVersion, ID3D11Device* device);

		void RegisterShaderBuilder();
		void RegisterShaderStager();

		void BuildVertexShader(std::string_view blobPath, ID3D11Device* device);
		void BuildPixelShader(std::string_view blobPath, ID3D11Device* device);
		void BuildGeometryShader(std::string_view blobPath, ID3D11Device* device);
		void BuildHullShader(std::string_view blobPath, ID3D11Device* device);
		void BuildDomainShader(std::string_view blobPath, ID3D11Device* device);
		void BuildComputeShader(std::string_view blobPath, ID3D11Device* device);

		

		BufferLayout m_Layout;
		ID3D11DeviceContext* m_DeviceContext;
		ShaderGroup m_ShaderGroup;

		DXGI_FORMAT GetFormat(ShaderDataType type);
	};
}

#endif // !D3D11_SHADER_HPP
