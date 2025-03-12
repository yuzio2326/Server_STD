#include "ShaderCompiler.h"
#include "Shader.h"
#include "RHI.h"

ENGINE_API FShaderCompilingManager* FShaderCompilingManager::Get(const bool bDestroy)
{
    static unique_ptr<FShaderCompilingManager> Instance = make_unique<FShaderCompilingManager>();
    if (bDestroy)
    {
        Instance.reset();
        return nullptr;
    }

    return Instance.get();
}

FShaderCompilingManager::FShaderCompilingManager()
{
    for (auto It : FGlobalShaderMap::ShaderTypes)
    {
        FShaderType* ShaderType = It.second;       
        
        TObjectPtr<FShader> NewShader;
        if (GDynamicRHI->RHICompileShader(ShaderType, NewShader))
        {
            FGlobalShaderMap::Shaders.emplace(ShaderType->TypeIndex, NewShader);
        }
    }
}
