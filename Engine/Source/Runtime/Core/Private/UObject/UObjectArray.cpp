#include "UObject/UObjectArray.h"
#include "boost/pool/pool.hpp"

FUObjectArray& GetObjectArray()
{
	static FUObjectArray GUObjectArray;
	return GUObjectArray;
}

struct FMemoryPool
{
public:
	FMemoryPool(const string_view InTypeName, const uint64 InSizePerOne)
		: TypeName(InTypeName), SizePerOne(InSizePerOne), Pool(SizePerOne) { }

	void* Malloc()
	{
		void* NewAddress = Pool.malloc();
		return NewAddress;
	}

	void Free(void* InAddress)
	{
		Pool.free(InAddress);
	}

private:
	string_view TypeName;
	uint64 SizePerOne;
	boost::pool<> Pool;
};

void FUObjectArray::Create(const type_info& InTypeInfo, const uint64 InSizePerOne)
{
	if (!MemoryPools[InTypeInfo.hash_code()])
	{
		MemoryPools[InTypeInfo.hash_code()] = new FMemoryPool(InTypeInfo.name(), InSizePerOne);
	}
}

void* FUObjectArray::Malloc(const type_info& InTypeInfo)
{
	return MemoryPools[InTypeInfo.hash_code()]->Malloc();
}

void FUObjectArray::Free(const type_info& InTypeInfo, void* InAddress)
{
	MemoryPools[InTypeInfo.hash_code()]->Free(InAddress);
}

void FUObjectArray::Destroy()
{
	for (auto& It : MemoryPools)
	{
		delete It.second;
	}
	MemoryPools.clear();
}