#pragma once
#include "CoreTypes.h"
#include "UObjectMacros.h"
#include "NameTypes.h"
#include "EnginePtr.h"

class UClass;
class UObject;
class CORE_API UObjectBase : public std::enable_shared_from_this<UObjectBase>
{
public:
	UObjectBase();
	UObjectBase(EObjectFlags InObjectFlags, UClass* InClass, UObject* InOuter, FName InName);
	virtual ~UObjectBase();

	EObjectFlags GetFlags() const { return ObjectFlags; }
	UObject* GetOuter() const { return OuterPrivate; }
	UClass* GetClass() const { return ClassPrivate; }
	
	/**
	 * Outer 체인을 따라 특정 유형의 다음 객체를 검색합니다. (T는 UObject에서 파생되어야 합니다)
	 *
	 * @param   검색할 대상 클래스
	 * @return  이 객체의 Outer 체인에서 적절한 유형의 첫 번째 객체에 대한 포인터.
	 */
	UObject* GetTypedOuter(UClass* Target) const;

	/**
	 * Outer 체인을 따라 특정 유형의 다음 객체를 검색합니다. (T는 UObject에서 파생되어야 합니다)
	 *
	 * @return  이 객체의 Outer 체인에서 적절한 유형의 첫 번째 객체에 대한 포인터.
	 */
	template<typename T>
	T* GetTypedOuter() const
	{
		return (T*)GetTypedOuter(T::StaticClass());
	}


	/**
	* Returns the name of this object (with no path information)
	*
	* @return Name of the object.
	*/
	FORCEINLINE FString GetName() const
	{
		return GetFName().ToString();
	}

	/** Returns the logical name of this object */
	FORCEINLINE FName GetFName() const
	{
		return NamePrivate;
	}

private:
	EObjectFlags ObjectFlags;
	UClass* ClassPrivate;
	UObject* OuterPrivate;

	/** Name of this object */
	FName NamePrivate;
};