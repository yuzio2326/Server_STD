#pragma once
#include "CoreTypes.h"

struct FMemoryPool;
class CORE_API FUObjectArray
{
public:
	void Create(const type_info& InTypeInfo, const uint64 InSizePerOne);
	void* Malloc(const type_info& InTypeInfo);
	void Free(const type_info& InTypeInfo, void* InAddress);
	void Destroy();

private:
	unordered_map<FHashKey, FMemoryPool*> MemoryPools;
};

CORE_API FUObjectArray& GetObjectArray();