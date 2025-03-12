#pragma once
#include "CoreTypes.h"
//#include "EngineWeakPtr.h"
//
//bool CORE_API IsEngineExitRequested();
//
//void CORE_API RequestEngineExit(const FString ReasonString);
//
class UClass;
class UObject;
struct FStaticConstructObjectParameters;

class CORE_API FObjectInitializer
{
	friend class UObject;
	friend struct FStaticConstructObjectParameters;

public:
	// FUObjectThreadContext::Get().TopInitializer(); 이부분 대체
	static inline TArray<FObjectInitializer*> InitializerStack;

	/**
	* 이 클래스의 모든 인스턴스 내에서 인스턴스화될 컴포넌트 또는 서브오브젝트를 생성합니다.
	* @param   Outer                        서브오브젝트를 생성할 외부 객체
	* @param   SubobjectName                새 컴포넌트의 이름
	* @param   ReturnType                   반환 형식의 클래스, 모든 오버라이드는 이 형식이어야 합니다.
	* @param   ClassToConstructByDefault    파생 클래스가 오버라이드하지 않은 경우, 이 형식의 컴포넌트를 생성합니다.
	* @param   bIsRequired                  컴포넌트가 필수적이며, DoNotCreateDefaultSubobject가 지정되었더라도 항상 생성될 경우 true
	* @param   bIsTransient                 컴포넌트가 일시적인 속성에 할당되는 경우 true
	*/
	UObject* CreateDefaultSubobject(UObject* Outer, FName SubobjectFName, const UClass* ReturnType, /*const*/ UClass* ClassToCreateByDefault, bool bIsRequired = true, bool bIsTransient = false) const;

	/**
	 * 기본 클래스에서 정의된 서브오브젝트에 사용할 클래스를 설정합니다. 이 클래스는 기본 클래스에서 사용하는 클래스의 서브클래스여야 합니다.
	 * @param   SubobjectName   새로운 컴포넌트 또는 서브오브젝트의 이름
	 * @param   Class           지정된 서브오브젝트 또는 컴포넌트에 사용할 클래스
	 */
	const FObjectInitializer& SetDefaultSubobjectClass(FName SubobjectName, /*const*/ UClass* Class) const
	{
		//AssertIfSubobjectSetupIsNotAllowed(SubobjectName);
		SubobjectOverrides.Add(SubobjectName, Class);
		return *this;
	}


	/**
	 * 기본 클래스에서 정의된 서브오브젝트에 사용할 클래스를 설정합니다. 이 클래스는 기본 클래스에서 사용하는 클래스의 서브클래스여야 합니다.
	 * @param   SubobjectName   새로운 컴포넌트 또는 서브오브젝트의 이름
	 */
	template<class T>
	const FObjectInitializer& SetDefaultSubobjectClass(FName SubobjectName) const
	{
		return SetDefaultSubobjectClass(SubobjectName, T::StaticClass());
	}

private:
	/**  파생 클래스에서 오버라이드를 관리하기 위한 작은 도우미 구조체 **/
	struct FOverrides
	{
		/**  오버라이드를 추가하고, 그것이 적법한지 확인 **/
		void Add(FName InComponentName, /*const*/ UClass* InComponentClass/*, const TArrayView<const FName>* FullPath = nullptr*/);

		///**  잠재적으로 중첩된 오버라이드를 추가하고, 그것이 적법한지 확인 **/
		//void Add(FStringView InComponentPath, const UClass* InComponentClass);

		///**  잠재적으로 중첩된 오버라이드를 추가하고, 그것이 적법한지 확인 **/
		//void Add(TArrayView<const FName> InComponentPath, const UClass* InComponentClass, const TArrayView<const FName>* FullPath = nullptr);

		struct FOverrideDetails
		{
			/*const*/ UClass* Class = nullptr;
			FOverrides* SubOverrides = nullptr;
		};

		/**  오버라이드를 검색하거나, 파생 클래스에서 이 항목을 제거한 경우 TClassToConstructByDefault::StaticClass 또는 nullptr 반환 **/
		FOverrideDetails Get(FName InComponentName, const UClass* ReturnType, UClass* ClassToConstructByDefault, bool bOptional) const;

	private:
		static bool IsLegalOverride(const UClass* DerivedComponentClass, const UClass* BaseComponentClass);

		/**  오버라이드를 검색 **/
		int32 Find(FName InComponentName) const
		{
			for (int32 Index = 0; Index < Overrides.size(); Index++)
			{
				if (Overrides[Index].ComponentName == InComponentName)
				{
					return Index;
				}
			}
			return INDEX_NONE;
		}
		/**  오버라이드 배열의 요소 **/
		struct FOverride
		{
			FName ComponentName;
			/*const */UClass* ComponentClass = nullptr;
			unique_ptr<FOverrides> SubOverrides;
			bool bDoNotCreate = false;
			FOverride(FName InComponentName)
				: ComponentName(InComponentName)
			{
			}

			FOverride& operator=(const FOverride& Other)
			{
				ComponentName = Other.ComponentName;
				ComponentClass = Other.ComponentClass;
				SubOverrides = (Other.SubOverrides ? make_unique<FOverrides>(*Other.SubOverrides) : nullptr);
				bDoNotCreate = Other.bDoNotCreate;
				return *this;
			}

			FOverride(const FOverride& Other)
			{
				*this = Other;
			}

			FOverride(FOverride&&) = default;
			FOverride& operator=(FOverride&&) = default;
		};
		/**  오버라이드 배열 **/
		TArray<FOverride> Overrides;
	};

public:
	// FObjectInitializer를 사용해서 신규로 만들어진 UObject 객체
	TObjectPtr<UObject>& SharedObj;

	UClass* Class = nullptr;
	/** Object this object resides in. */
	UObject* OuterPrivate = nullptr;
	EObjectFlags ObjectFlags = EObjectFlags::RF_NoFlags;
	FName Name;

	// FAllocator 내부에서 MamoryPool로 부터 얻어온 주소(shared_ptr 크기를 고려해서 계산된 주소)
	UObject* Obj = nullptr;
	/** 속성을 복사할 객체 **/
	UObject* ObjectArchetype = nullptr;
	/**  If true, initialize the properties **/
	bool bShouldInitializePropsFromArchetype = false;
	/**  파생 클래스에서 오버라이드할 컴포넌트 클래스의 목록 **/
	mutable FOverrides SubobjectOverrides;

	UObject* GetObj() const { return Obj; }

	FObjectInitializer(TObjectPtr<UObject>& InObj, const FStaticConstructObjectParameters& StaticConstructParams);
	~FObjectInitializer();

	/**
	 * 객체 속성을 이진 방식으로 0 또는 기본값으로 초기화합니다.
	 *
	 * @param   Obj                 데이터를 초기화할 객체
	 * @param   DefaultsClass       데이터 초기화에 사용할 클래스
	 * @param   DefaultData         초기화에 사용할 원본 데이터를 포함하는 버퍼
	 * @param   bCopyTransientsFromClassDefaults    true인 경우, 클래스 기본값에서 일시적인 데이터를 복사하고, 그렇지 않으면 DefaultData에서 일시적인 데이터를 복사합니다.
	 */
	static void InitProperties(UObject* Obj, UClass* DefaultsClass, UObject* DefaultData, bool bCopyTransientsFromClassDefaults);

	/**
	 * 생성된 UObject를 속성 초기화, 
	 * 서브 오브젝트 인스턴싱/초기화 등을 통해 최종 확정합니다.
	 */
	void PostConstructInit();
};

CORE_API TObjectPtr<UObject> StaticConstructObject_Internal(FStaticConstructObjectParameters& Params);

// wstring -> UTF8
CORE_API string to_string(const FString& InString);
// wstring -> UTF8
CORE_API string to_string(FStringView InString);
// UTF8 -> wstring
CORE_API FString to_wstring(string_view InString);
#define ANSI_TO_TCHAR to_wstring
#define TCHAR_TO_ANSI to_string

CORE_API uint64 operator""_hash(const ANSICHAR * NewString, size_t);
CORE_API uint64 operator""_hash(const WIDECHAR * NewString, size_t);
CORE_API uint64 Hash(const ANSICHAR* NewString);
CORE_API uint64 Hash(const WIDECHAR* NewString);
//
///**
// * Get default object of a class.
// * @see UClass::GetDefaultObject()
// */
//template< class T >
//inline const T* GetDefault()
//{
//	return (const T*)T::StaticClass()->GetDefaultObject();
//}
//
//extern CORE_API map<UClass*, vector<EnginePtr<UObject>>> ObjectMap;
//
///** Enum used in StaticDuplicateObject() and related functions to describe why something is being duplicated */
//namespace EDuplicateMode
//{
//	enum Type
//	{
//		/** No specific information about the reason for duplication */
//		Normal,
//		/** Object is being duplicated as part of a world duplication */
//		PlayWorld,
//		/** Object is being duplicated as part of the process for entering Play In Editor */
//		PIE
//	};
//};
//
//CORE_API shared_ptr<UObject> StaticDuplicateObject(UObject* SourceObject, UObject* DestOuter, const FString DestName, EDuplicateMode::Type DuplicateMode = EDuplicateMode::Normal);


#include "UObjectArray.h"
#include "Object.h"

template <class _Ty>
class FAllocator
{
public:
	static_assert(!is_const_v<_Ty>, "The C++ Standard forbids containers of const elements "
		"because allocator<const T> is ill-formed.");
	static_assert(!is_function_v<_Ty>, "The C++ Standard forbids allocators for function elements "
		"because of [allocator.requirements].");
	static_assert(!is_reference_v<_Ty>, "The C++ Standard forbids allocators for reference elements "
		"because of [allocator.requirements].");

	using _From_primary = FAllocator;

	using value_type = _Ty;
	using size_type = size_t;
	using difference_type = ptrdiff_t;

	using propagate_on_container_move_assignment = true_type;
	using is_always_equal _CXX20_DEPRECATE_IS_ALWAYS_EQUAL = true_type;
	FAllocator(FObjectInitializer* InObjectInitializer) noexcept :
		Data(InObjectInitializer)
	{
	}
	constexpr FAllocator(const FAllocator&) noexcept = default;
	template <class _Other>
	constexpr FAllocator(const FAllocator<_Other>& InOther) noexcept
		: Data(InOther.Data.ObjectInitializer)
	{
	}
	_CONSTEXPR20 ~FAllocator() = default;
	_CONSTEXPR20 FAllocator& operator=(const FAllocator&) = default;

	_NODISCARD_RAW_PTR_ALLOC _CONSTEXPR20 __declspec(allocator) _Ty* allocate(_CRT_GUARDOVERFLOW const size_t /*_Count*/) {
		static_assert(sizeof(value_type) > 0, "value_type must be complete before calling allocate.");
		_Ty* Pointer = (_Ty*)GetObjectArray().Malloc(Data.ObjectInitializer->Class->ClassTypeInfo);
		return Pointer;
	}

	template <class _Objty, class... _Types>
	_CXX17_DEPRECATE_OLD_ALLOCATOR_MEMBERS void construct(_Objty* const _Ptr, _Types&&... _Args)
	{
		Data.ObjectInitializer->Obj = _Ptr;
		new(Data.ObjectInitializer->GetObj())UObjectBase(
			Data.ObjectInitializer->ObjectFlags,
			Data.ObjectInitializer->Class,
			Data.ObjectInitializer->OuterPrivate,
			Data.ObjectInitializer->Name
		);
		_Objty::__DefaultConstructor(*Data.ObjectInitializer);
	}

	template< class U >
	_CONSTEXPR20 void destroy(U* p)
	{
		UClass* Class = p->GetClass();
		Data.DestructorClass = Class;
		_ASSERT(Class);
		p->~U();
	}
	_CONSTEXPR20 void deallocate(_Ty* const _Ptr, const size_t _Count) {
		_STL_ASSERT(_Ptr != nullptr || _Count == 0, "null pointer cannot point to a block of non-zero size");
		_STL_ASSERT(_Count == 1, "error");
		GetObjectArray().Free(Data.DestructorClass->ClassTypeInfo, _Ptr);
	}

public:
	union FData
	{
		FObjectInitializer* ObjectInitializer;
		UClass* DestructorClass;
	};
	FData Data;
};

/**
 * 이 구조체는 StaticConstructObject_Internal() 메서드에 매개변수 값을 전달하는 데 사용됩니다. 생성자 매개변수만 유효하면 되며,
 * 다른 모든 멤버는 선택 사항입니다.
 */
struct CORE_API FStaticConstructObjectParameters
{
	UClass* Class = nullptr;
	UObject* Outer = nullptr;
	FName Name;
	EObjectFlags SetFlags = RF_NoFlags;

	/**
	 * 지정된 경우, 이 객체의 속성 값이 새 객체로 복사되며, 새 객체의 ObjectArchetype 값이 이 객체로 설정됩니다.
	 * 만약 nullptr인 경우, 클래스 기본 객체(class default object)가 대신 사용됩니다.
	 */
	UObject* Template = nullptr;
private:
	FObjectInitializer::FOverrides* SubobjectOverrides = nullptr;

public:
	FStaticConstructObjectParameters(UClass* InClass);

	friend FObjectInitializer;
};

template<typename T>
TObjectPtr<T> NewObject(UObject* Outer, UClass* Class = nullptr, FName Name = NAME_NONE, EObjectFlags Flags = RF_NoFlags, UObject* Template = nullptr)
{
	if (!Class)
	{
		Class = T::StaticClass();
	}

	FStaticConstructObjectParameters Params(Class);
	Params.Outer = Outer;
	Params.Name = Name;
	Params.SetFlags = Flags;
	Params.Template = Template;

	return Cast<T>(StaticConstructObject_Internal(Params));
}

TEnginePtr<UObject> CORE_API FindObject(const FString& InClassName, const FString& InObjectName, UObject* Outer = nullptr);

/**
 * 클래스의 기본 오브젝트를 가져옵니다.
 * @see UClass::GetDefaultObject()
 */
template< class T >
inline const T* GetDefault()
{
	return (const T*)T::StaticClass()->GetDefaultObject();
}

/** StaticDuplicateObject() 및 관련 함수에서 사용되어 객체가 복제되는 이유를 설명하는 열거형 */
namespace EDuplicateMode
{
	enum Type
	{
		/** 복제 이유에 대한 구체적인 정보 없음 */
		Normal,
		/** 월드 복제의 일환으로 객체가 복제됨 */
		World,
		/** Play In Editor 단계의 일환으로 객체가 복제됨 */
		PIE
	};
};


/**
 * SourceObject를 주어진 Outer 및 Name을 사용하여 복사본을 생성하고, SourceObject에 포함된 모든 객체의 복사본도 생성합니다.
 * SourceOuter 또는 RootObject가 참조하고 SourceOuter에 포함된 모든 객체도 복사하고, SourceOuter에 상대적인 이름을 유지합니다.
 * 복제된 객체에 대한 모든 참조는 자동으로 객체의 복사본으로 대체됩니다.
 *
 * @param   SourceObject    복제할 객체
 * @param   DestOuter       SourceObject의 복사본에 대해 Outer로 사용할 객체
 * @param   DestName        SourceObject의 복사본에 사용할 이름, 없다면 자동으로 생성됩니다
 * @param   FlagMask        객체 복사본에 전파해야 할 EObjectFlags의 비트 마스크. 결과 객체 복사본은 해당 객체 플래그들만 복사됩니다.
 * @param   DestClass       대상 객체에 지정할 선택적 클래스. SOURCE OBJECT와 직렬화 호환되어야 합니다!!!
 * @param   InternalFlagsMask  객체 복사본에 전파해야 할 EInternalObjectFlags의 비트 마스크.
 *
 * @return  SourceObject의 복사본.
 *
 * @deprecated 이 버전은 StaticDuplicateObjectEx를 권장하므로 더 이상 사용되지 않습니다.
 */
CORE_API TObjectPtr<UObject> StaticDuplicateObject(UObject* SourceObject, UObject* DestOuter, const FName DestName = NAME_None, EDuplicateMode::Type DuplicateMode = EDuplicateMode::Normal/*, EInternalObjectFlags InternalFlagsMask = EInternalObjectFlags_AllFlags*/);
