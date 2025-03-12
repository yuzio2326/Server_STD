#include "UObject/Class.h"
#include "UObject/UObjectArray.h"
#include "Logging/Logger.h"

CORE_API map<FString, UClass*>& GetClassMap()
{
	static map<FString, UClass*> ClassMap;
	return ClassMap;
}

UClass* UClass::FindClass(FStringView InClassName)
{
	const FString Key = InClassName.data();
	if (!GetClassMap().contains(Key))
	{
		E_LOG(Error, TEXT("Can not find Class: {}"), InClassName);
		return nullptr;
	}

	UClass* Class = GetClassMap()[Key];
	return Class;
}

TArray<UClass*> UClass::GetAllSubclassOfClass(UClass* InBaseClass)
{
	TArray<UClass*> Result;
	for (auto It : GetClassMap())
	{
		UClass* Class = It.second;
		if (Class->IsChildOf(InBaseClass))
		{
			Result.emplace_back(Class);
		}
	}
	return Result;
}

bool UClass::IsChildOf(const UClass* SomeBase) const
{
	_ASSERT(this);

	if (SomeBase == nullptr)
	{
		return false;
	}

	bool bOldResult = false;
	for (const UClass* TempClass = this; TempClass; TempClass = TempClass->GetSuperClass())
	{
		if (TempClass == SomeBase)
		{
			bOldResult = true;
			break;
		}
	}

	return bOldResult;
}

UClass::UClass(FString InClassName, const type_info& InClassTypeInfo, const uint64 InClassSize,
	ClassConstructorType InClassConstructorType, StaticClassFunctionType InSuperClassFunction)
	: SuperClass(nullptr)
	, ClassName(InClassName)
	, ClassTypeInfo(InClassTypeInfo)
	, ClassSize(InClassSize)
	, ClassConstructor(InClassConstructorType)
{
	if (InSuperClassFunction)
	{
		SuperClass = InSuperClassFunction();
	}
}

UObject* UClass::CreateDefaultObject()
{
	_ASSERT(!ClassDefaultObject);

	FStaticConstructObjectParameters StaticConstructObjectParameters(this);
	StaticConstructObjectParameters.SetFlags = EObjectFlags::RF_ClassDefaultObject;
	StaticConstructObjectParameters.Name = FName(ClassName);
	ClassConstructor(FObjectInitializer(ClassDefaultObject, StaticConstructObjectParameters));

	return ClassDefaultObject.get();
}

void UClass::InternalCreateDefaultObjectWrapper() const
{
	const_cast<UClass*>(this)->CreateDefaultObject();
}

UClass* GetPrivateStaticClassBody(FString InClassName, 
	UClass::ClassConstructorType InClassConstructor,
	UClass::StaticClassFunctionType InSuperClassFn,
	function<void()> InConstructFProperties,
	const type_info& InClassTypeInfo, const uint64 InClassSize)
{
	// UE 예시)
	// GetPrivateStaticClassBody
	// ...
	// - Z_Construct_UClass_AXXX 에서 UProperty 등록 과정을 거치며 FProperty를 생성하고 있다
	// 	if (!Z_Registration_Info_UClass_AStudyCharacter.OuterSingleton)
	/*{
		UECodeGen_Private::ConstructUClass(Z_Registration_Info_UClass_AStudyCharacter.OuterSingleton, Z_Construct_UClass_AStudyCharacter_Statics::ClassParams);
	}*/
	// 
	if (InConstructFProperties)
	{
		InConstructFProperties();
	}
	GetObjectArray().Create(typeid(UClass), sizeof(UClass));

	const uint64 SharedPtrSize = sizeof(_Ref_count_obj_alloc3<UObject, FAllocator<UObject>>) - sizeof(UObject);
	GetObjectArray().Create(InClassTypeInfo, SharedPtrSize + InClassSize);

	UClass* ReturnClass = (UClass*)GetObjectArray().Malloc(typeid(UClass));
	::new(ReturnClass)UObjectBase(EObjectFlags::RF_Class, nullptr, nullptr, FName(InClassName));
	ReturnClass = ::new (ReturnClass)
		UClass
		(
			InClassName, 
			InClassTypeInfo, 
			InClassSize, 
			InClassConstructor, 
			InSuperClassFn
		);

	GetClassMap().emplace(InClassName, ReturnClass);

	return ReturnClass;
}
