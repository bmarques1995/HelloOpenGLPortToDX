#ifndef SHADER_MANAGER_HPP
#define SHADER_MANAGER_HPP

#include <sstream>
#include <initializer_list>
#include "ShaderCompiler.hpp"

namespace APILearning
{
	struct ShaderDetails
	{
		std::string_view Entrypoint;
		SHADER_KIND ShaderKind;

		ShaderDetails(SHADER_KIND shaderKind, std::string_view entrypoint = "main") :
			ShaderKind(shaderKind), Entrypoint(entrypoint)
		{
		
		}
	};
	class ShaderGroup
	{
	public:
		const std::vector<ShaderDetails>& GetShaderDetails() const { return m_ShadersDetails; }
		std::string_view GetBasePath() const { return m_BaseShaderPath; }
		HLSL_VERSION GetHLSLVersion() const { return m_HLSLVersion; }

		ShaderGroup(std::string_view baseShaderPath, HLSL_VERSION hlslVersion, const std::initializer_list<ShaderDetails>& shadersDetails) :
			m_ShadersDetails(shadersDetails), m_HLSLVersion(hlslVersion), m_BaseShaderPath(baseShaderPath)
		{
		
		}

	private:
		std::string_view m_BaseShaderPath;
		HLSL_VERSION m_HLSLVersion;
		std::vector<ShaderDetails> m_ShadersDetails;
	};
	class ShaderManager
	{
	public:
		static void CompileShaderFamily(const ShaderGroup& shaderGroup);
		static const std::string& BuildBlobFilename(SHADER_KIND shaderKind, HLSL_VERSION hlslVersion, std::string_view baseShaderPath);
	private:
		static std::string s_Filename;
	};
}

#endif // SHADER_MANAGER_HPP
