#include "UObject/UObjectBase.h"
#include "UObject/Object.h"

extern CORE_API map<UClass*, multimap<FString, TEnginePtr<UObject>>> ObjectMap;

#pragma warning(disable:26495)
UObjectBase::UObjectBase()
	: NamePrivate(NoInit)
{
}
#pragma warning(default:26495)

UObjectBase::UObjectBase(EObjectFlags InObjectFlags, UClass* InClass, UObject* InOuter, FName InName)
	: ObjectFlags(InObjectFlags), ClassPrivate(InClass), OuterPrivate(InOuter), NamePrivate(InName)
{
}

UObjectBase::~UObjectBase()
{
	auto& MultiMap = ObjectMap[GetClass()];
	auto Range = MultiMap.equal_range(GetName());
	for (auto It = Range.first; It != Range.second; )
	{
		if (It->second && It->second->GetOuter() == GetOuter())
		{
			It = MultiMap.erase(It);
			break;
		}
		else if (!It->second)
		{
			It = MultiMap.erase(It);
		}
		else
		{
			++It;
		}
	}
}
/**
 * Outer 체인을 따라 특정 유형의 다음 객체를 검색합니다. (T는 UObject에서 파생되어야 합니다)
 *
 * @param   검색할 대상 클래스
 * @return  이 객체의 Outer 체인에서 적절한 유형의 첫 번째 객체에 대한 포인터.
 */
UObject* UObjectBase::GetTypedOuter(UClass* Target) const
{
	//ensureMsgf(Target != UPackage::StaticClass(), TEXT("Calling GetTypedOuter to retrieve a package is now invalid, you should use GetPackage() instead."));

	UObject* Result = NULL;
	for (UObject* NextOuter = GetOuter(); Result == NULL && NextOuter != NULL; NextOuter = NextOuter->GetOuter())
	{
		if (NextOuter->IsA(Target))
		{
			Result = NextOuter;
		}
	}
	return Result;
}