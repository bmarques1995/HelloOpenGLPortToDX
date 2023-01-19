#include "utils/ShaderCompiler.hpp"
#include "utils/FileController.hpp"
#include <sstream>

std::string APILearning::ShaderCompiler::s_Target; 
std::string APILearning::ShaderCompiler::s_InputFile;
std::string APILearning::ShaderCompiler::s_OutputFile;

std::unordered_map<APILearning::SHADER_KIND, std::string_view> APILearning::ShaderCompiler::s_ShaderKindStringMap =
{ 
	{SHADER_KIND::D3D_SHADER_KIND_VERTEX, "vs"},
	{SHADER_KIND::D3D_SHADER_KIND_PIXEL, "ps"},
	{SHADER_KIND::D3D_SHADER_KIND_GEOMETRY, "gs"},
	{SHADER_KIND::D3D_SHADER_KIND_HULL, "hs"},
	{SHADER_KIND::D3D_SHADER_KIND_DOMAIN, "ds"},
	{SHADER_KIND::D3D_SHADER_KIND_COMPUTE, "cs"},
};
std::unordered_map<APILearning::HLSL_VERSION, std::string_view> APILearning::ShaderCompiler::s_HLSLVersionStringMap =
{
	{HLSL_VERSION::HLSL_4_0, "_4_0"},
	{HLSL_VERSION::HLSL_5_0, "_5_0"},
	{HLSL_VERSION::HLSL_5_1, "_5_1"},
};

APILearning::COMPILE_ERRORS APILearning::ShaderCompiler::CompileFromFile(std::string_view baseShaderPath, std::string_view entrypoint, SHADER_KIND shaderKind, HLSL_VERSION hlslVersion, uint32_t optimizationLevel, uint32_t extraFlags)
{
	BuildInputFile(shaderKind, baseShaderPath);
	std::string shaderSource;
	if (!FileController::ReadTextFile(s_InputFile, &shaderSource))
		return COMPILE_ERRORS::COMPILER_FILE_NOT_FOUND;
	BuildTarget(shaderKind, hlslVersion);

	ID3DBlob* shaderBlob;
	ID3DBlob* errorBlob;

	HRESULT hr = D3DCompile(shaderSource.c_str(), shaderSource.size(), nullptr, nullptr, nullptr, entrypoint.data(), s_Target.c_str(), optimizationLevel, extraFlags, &shaderBlob, &errorBlob);

	if (errorBlob)
	{
		if (errorBlob->GetBufferSize())
		{
			OutputDebugStringA((const char*)errorBlob->GetBufferPointer());
			OutputDebugStringA("\n");
		}
		errorBlob->Release();
		return COMPILE_ERRORS::COMPILER_ERROR_SOURCE_CODE;
	}

	BuildOutputFile(shaderKind, hlslVersion, baseShaderPath);

	FileController::WriteBinFile(s_OutputFile, (std::byte*)shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize());

	shaderBlob->Release();
    return COMPILE_ERRORS::COMPILER_COMPILE_SUCCESS;
}

void APILearning::ShaderCompiler::BuildTarget(SHADER_KIND shaderKind, HLSL_VERSION hlslVersion)
{
	std::stringstream target;
	target << s_ShaderKindStringMap[shaderKind];
	target << s_HLSLVersionStringMap[hlslVersion];

	s_Target = target.str();
}

void APILearning::ShaderCompiler::BuildInputFile(SHADER_KIND shaderKind, std::string_view baseShaderPath)
{
	std::stringstream inputFile;
	inputFile << baseShaderPath;
	inputFile << "." << s_ShaderKindStringMap[shaderKind];
	inputFile << ".hlsl";
	s_InputFile = inputFile.str();
}

void APILearning::ShaderCompiler::BuildOutputFile(SHADER_KIND shaderKind, HLSL_VERSION hlslVersion, std::string_view baseShaderPath)
{
	std::stringstream outputFile;
	outputFile << baseShaderPath;
	outputFile << "." << s_ShaderKindStringMap[shaderKind] << s_HLSLVersionStringMap[hlslVersion];
	outputFile << ".dxbc";
	s_OutputFile = outputFile.str();
}
