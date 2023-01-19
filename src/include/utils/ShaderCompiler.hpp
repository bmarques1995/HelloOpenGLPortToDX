#ifndef SHADER_COMPILER_HPP
#define SHADER_COMPILER_HPP

#include <string>
#include <cstdint>
#include <d3dcompiler.h>
#include <unordered_map>

namespace APILearning
{
	enum class COMPILE_ERRORS
	{
		COMPILER_COMPILE_SUCCESS = 0,
		COMPILER_FILE_NOT_FOUND,
		COMPILER_ERROR_SOURCE_CODE
		
	};

	enum class SHADER_KIND
	{
		D3D_SHADER_KIND_VERTEX = 0,
		D3D_SHADER_KIND_PIXEL,
		D3D_SHADER_KIND_GEOMETRY,
		D3D_SHADER_KIND_HULL,
		D3D_SHADER_KIND_DOMAIN,
		D3D_SHADER_KIND_COMPUTE

	};

	enum class HLSL_VERSION
	{
		HLSL_4_0,
		HLSL_5_0,
		HLSL_5_1
	};

	class ShaderCompiler
	{
		friend class ShaderManager;
	public:
		static COMPILE_ERRORS CompileFromFile(std::string_view baseShaderPath, std::string_view entrypoint, SHADER_KIND shaderKind, HLSL_VERSION hlslVersion, uint32_t optimizationLevel = D3DCOMPILE_DEBUG, uint32_t extraFlags = 0);
	private:
		static std::string s_Target;
		static std::string s_InputFile;
		static std::string s_OutputFile;

		static std::unordered_map<SHADER_KIND, std::string_view> s_ShaderKindStringMap;
		static std::unordered_map<HLSL_VERSION, std::string_view> s_HLSLVersionStringMap;

		static void BuildTarget(SHADER_KIND shaderKind, HLSL_VERSION hlslVersion);
		static void BuildInputFile(SHADER_KIND shaderKind, std::string_view baseShaderPath);
		static void BuildOutputFile(SHADER_KIND shaderKind, HLSL_VERSION hlslVersion, std::string_view baseShaderPath);
		
	};
}

#endif // !1
