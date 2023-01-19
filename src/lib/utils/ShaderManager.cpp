#include "utils/ShaderManager.hpp"
#include "utils/FileController.hpp"
#include <cstdint>

std::string APILearning::ShaderManager::s_Filename;

void APILearning::ShaderManager::CompileShaderFamily(const ShaderGroup& shaderGroup)
{
    bool debugMode;
	uint32_t compileFlags;
#ifdef _DEBUG
	compileFlags = D3DCOMPILE_DEBUG;
    debugMode = true;
#else
	compileFlags = D3DCOMPILE_OPTIMIZATION_LEVEL3;
    debugMode = false;
#endif // _DEBUG

	auto details = shaderGroup.GetShaderDetails();
	bool blobExists = true;
	
	if(details.size() > 0)
		blobExists = FileController::FileExists(BuildBlobFilename(details[0].ShaderKind, shaderGroup.GetHLSLVersion(), shaderGroup.GetBasePath()));

	if (debugMode || !blobExists)
	{
		for (size_t i = 0; i < details.size(); i++)
		{
			ShaderCompiler::CompileFromFile(shaderGroup.GetBasePath(), details[i].Entrypoint, details[i].ShaderKind, shaderGroup.GetHLSLVersion(), compileFlags);
		}
	}
}

const std::string& APILearning::ShaderManager::BuildBlobFilename(SHADER_KIND shaderKind, HLSL_VERSION hlslVersion, std::string_view baseShaderPath)
{
	std::stringstream outputFile;
	outputFile << baseShaderPath;
	outputFile << "." << ShaderCompiler::s_ShaderKindStringMap[shaderKind] << ShaderCompiler::s_HLSLVersionStringMap[hlslVersion];
	outputFile << ".dxbc";
	s_Filename = outputFile.str();
	return s_Filename;
}

