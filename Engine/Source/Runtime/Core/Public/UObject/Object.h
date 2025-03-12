#pragma once
#include "UObjectBase.h"
#include "Serialization/Archive.h"

class UClass;
class FObjectInitializer;

class CORE_API UObject : public UObjectBase
{
public:
	virtual void Serialize(FArchive& Ar);

	// 에디터에서 UPROPERTY 값을 변경 한 경우 및 Serialized load에서 호출 됩니다
	virtual void OnPropertyChanged(struct FProperty&) {}

public:
	/** 아래 템플릿을 위한 유틸리티 함수 */
	UObject* CreateDefaultSubobject(FName SubobjectFName, UClass* ReturnType, UClass* ClassToCreateByDefault, bool bIsRequired, bool bIsTransient);
	/**
	 * 이 클래스의 모든 인스턴스 내에서 인스턴스화될 컴포넌트 또는 서브오브젝트를 생성합니다.
	 * @param   TReturnType                 반환 형식의 클래스, 모든 오버라이드는 이 형식이어야 합니다.
	 * @param   SubobjectName               새 컴포넌트의 이름
	 * @param   bTransient                  컴포넌트가 일시적인 속성에 할당되는 경우 true로 설정합니다. 이는 컴포넌트 자체를 일시적으로 만들지는 않지만 부모 기본값을 상속받지 않게 합니다.
	 */
	template<class TReturnType>
	TReturnType* CreateDefaultSubobject(FName SubobjectName, bool bTransient = false)
	{
		UClass* ReturnType = TReturnType::StaticClass();
		return static_cast<TReturnType*>(CreateDefaultSubobject(SubobjectName, ReturnType, ReturnType, /*bIsRequired =*/ true, bTransient));
	}

public:
	/**
	 * C++ 생성자 호출 후 및 속성(구성에서 로드된 속성 포함)이 초기화된 후 호출됩니다.
	 * 이는 직렬화 또는 다른 설정이 이루어지기 전에 호출됩니다.
	 */
	virtual void PostInitProperties();

public:
	static void __DefaultConstructor(const FObjectInitializer& X);
	static UClass* StaticClass();

	template<class T>
	TObjectPtr<T> As()
	{
		if (!this)
		{
			_ASSERT(false);
			return nullptr;
		}
		return CastChecked<T>(shared_from_this()); 
	}

	/** 이 객체가 지정된 타입인지 여부를 반환합니다. */
	template <typename OtherClassType>
	FORCEINLINE bool IsA(OtherClassType SomeBase) const
	{
		// UObjectBaseUtility와 UClass 사이에 순환 종속성이 있습니다.
		// 이를 해결하기 위해 템플릿을 사용하여 아직 보지 않은 것을 인라인화할 수 있도록 합니다.
		// 이는 함수가 호출될 때까지 컴파일을 지연시키기 때문입니다.

		// 'static_assert'로 이 객체가 실제로 UClass 포인터인지 또는 변환 가능한지 확인합니다.
		const UClass* SomeBaseClass = SomeBase;
		(void)SomeBaseClass;
		_ASSERT(SomeBaseClass/*, TEXT("IsA(NULL)은 의미 있는 결과를 낼 수 없습니다")*/);

		const UClass* ThisClass = GetClass();

		// 컴파일러가 불필요한 nullptr 체크 분기를 하지 않도록 합니다.
		//UE_ASSUME(SomeBaseClass);
		//UE_ASSUME(ThisClass);

		return IsChildOfWorkaround(ThisClass, SomeBaseClass);
	}


	bool HasAnyFlags(const EObjectFlags FlagToCheck) const
	{
		return (GetFlags() & FlagToCheck);
	}

	/**
	 * 이 객체가 포함된 UWorld를 반환합니다.
	 * 기본적으로 Outer 체인을 따르지만, 그 방식이 작동하지 않을 경우에는 재정의되어야 합니다.
	 */
	virtual class UWorld* GetWorld() const;

private:
	template <typename ClassType>
	static FORCEINLINE bool IsChildOfWorkaround(const ClassType* ObjClass, const ClassType* TestCls)
	{
		return ObjClass->IsChildOf(TestCls);
	}

private:
	template<typename> friend class FAllocator;
};

template<class T1, class T2>
static TObjectPtr<T1> Cast(T2* InObj) { return std::dynamic_pointer_cast<T1>(InObj->shared_from_this()); }
template<class T1, class T2>
static TObjectPtr<T1> Cast(TObjectPtr<T2> InObj) { return std::dynamic_pointer_cast<T1>(InObj); }
template<class T1, class T2>
static TEnginePtr<T1> Cast(TEnginePtr<T2> InObj) { return std::dynamic_pointer_cast<T1>(InObj.lock()); }

template<class T1, class T2>
static TObjectPtr<T1> CastChecked(T2* InObj)
{
	TObjectPtr<T1> CastResult = std::dynamic_pointer_cast<T1>(InObj->shared_from_this());
	_ASSERT(CastResult.get());
	return CastResult;
}
template<class T1, class T2>
static T1* CastCheckedRaw(T2* InObj)
{
	T1* CastResult = dynamic_cast<T1*>(InObj);
	_ASSERT(CastResult);
	return CastResult;
}
template<class T1, class T2>
static TObjectPtr<T1> CastChecked(TObjectPtr<T2> InObj)
{
	TObjectPtr<T1> CastResult = std::dynamic_pointer_cast<T1>(InObj);
	_ASSERT(CastResult.get());
	return CastResult;
}
template<class T1, class T2>
static TEnginePtr<T1> CastChecked(TEnginePtr<T2> InObj)
{
	TEnginePtr<T1> CastResult = std::dynamic_pointer_cast<T1>(InObj.lock());
	CastResult.GetChecked();
	return CastResult;
}
