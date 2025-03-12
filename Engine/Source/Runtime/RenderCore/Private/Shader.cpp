#include "Shader.h"

FShaderType::FShaderType(const type_index ClassType, const TCHAR* InClassName, const FString InShaderFilePath,
	const char* InFunctionName, EShaderFrequency InShaderFrequency,
	FShaderConstructFunction InShaderConstructFunction)
	: TypeIndex(ClassType), ClassName(InClassName), ShaderFilePath(InShaderFilePath), FunctionName(InFunctionName), Frequency(InShaderFrequency)
	, ShaderConstructFunction(InShaderConstructFunction)
{
	FGlobalShaderMap::ShaderTypes.emplace(ClassType, this);
}

FShaderType::~FShaderType()
{
	FGlobalShaderMap::ShaderTypes.erase(TypeIndex);
}

FShader::~FShader()
{
}
