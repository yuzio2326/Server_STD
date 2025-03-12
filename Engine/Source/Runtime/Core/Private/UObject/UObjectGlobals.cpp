#include "UObject/UObjectGlobals.h"
#include "UObject/Class.h"
#include "UObject/Object.h"
#include "Logging/Logger.h"
#include "Serialization/Archive.h"
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>

bool					GIsRequestingExit = false;
bool CORE_API IsEngineExitRequested()
{
	return GIsRequestingExit;
}

CORE_API map<UClass*, multimap<FString, TEnginePtr<UObject>>> ObjectMap;
TEnginePtr<UObject> CORE_API FindObject(const FString& InClassName, const FString& InObjectName, UObject* Outer)
{
	UClass* Class = UClass::FindClass(InClassName);
	if (!Class) { return nullptr; }

	if (!ObjectMap[Class].contains(InObjectName)) { return nullptr; }

	auto Range = ObjectMap[Class].equal_range(InObjectName);
	for (auto it = Range.first; it != Range.second; ++it) 
	{
		if (Outer == nullptr)
		{
			uint64 Count = std::distance(Range.first, Range.second);
			if (Count != 1)
			{
				_ASSERT(false);
			}
			return it->second;
		}

		if (it->second->GetOuter() == Outer)
		{
			return it->second;
		}
	}

	return nullptr;
}

CORE_API TObjectPtr<UObject> StaticDuplicateObject(UObject* SourceObject, UObject* DestOuter, const FName DestName, EDuplicateMode::Type DuplicateMode)
{
	FStaticConstructObjectParameters Param(SourceObject->GetClass());
	Param.Outer = DestOuter;
	Param.Name = DestName;
	Param.SetFlags = SourceObject->GetFlags();
	Param.Template = SourceObject;

	TObjectPtr<UObject> DupRootObject = StaticConstructObject_Internal(Param);

	map<UObject*, TObjectPtr<UObject>> DuplicatedObjectAnnotation;

	std::stringstream Buffer;
	boost::archive::text_oarchive WriteArchive = boost::archive::text_oarchive(Buffer);
	FArchive Writer = FArchive(WriteArchive, DuplicatedObjectAnnotation, SourceObject->As<UObject>(), DupRootObject);

	TArray<TObjectPtr<UObject>> SerializedObjects;
	SerializedObjects.reserve(Writer.UnserializedObjects.size());

	while (Writer.UnserializedObjects.size())
	{
		TObjectPtr<UObject> Object = Writer.UnserializedObjects.top();
		Writer.UnserializedObjects.pop();

		Object->Serialize(Writer);
		SerializedObjects.push_back(Object);
	};

	boost::archive::text_iarchive ReadArchive = boost::archive::text_iarchive(Buffer);
	FArchive Reader = FArchive(ReadArchive, DuplicatedObjectAnnotation);
	for (uint32 ObjectIndex = 0; ObjectIndex < SerializedObjects.size(); ++ObjectIndex)
	{
		TObjectPtr<UObject> SerializedObject = SerializedObjects[ObjectIndex];
		TObjectPtr<UObject> DuplicatedObject = DuplicatedObjectAnnotation[SerializedObject.get()];

		if (!DuplicatedObject->HasAnyFlags(RF_ClassDefaultObject))
		{
			DuplicatedObject->Serialize(Reader);
		}
		else
		{
			_ASSERT(false);
		}
	}
	return DupRootObject;
}

//void CORE_API RequestEngineExit(const FString ReasonString)
//{
//	E_LOG(Trace, TEXT("{}"), ReasonString.c_str());
//	GIsRequestingExit = true;
//}
//
FStaticConstructObjectParameters::FStaticConstructObjectParameters(UClass* InClass)
	: Class(InClass)
	, Name(NAME_None)
{
	if (!Class)
	{
		//E_LOG(Error, TEXT("Class is nullptr"))
	}
}

UObject* FObjectInitializer::CreateDefaultSubobject(UObject* Outer, FName SubobjectFName, const UClass* ReturnType, /*const*/ UClass* ClassToCreateByDefault, bool bIsRequired, bool bIsTransient) const
{
	if (SubobjectFName == NAME_None)
	{
		E_LOG(Fatal, TEXT("Illegal default subobject name: {}"), SubobjectFName.ToString());
	}
	TObjectPtr<UObject> Result = nullptr;
	FOverrides::FOverrideDetails ComponentOverride = SubobjectOverrides.Get(SubobjectFName, ReturnType, ClassToCreateByDefault, !bIsRequired);
	/*const*/ UClass* OverrideClass = ComponentOverride.Class;
	if (OverrideClass)
	{
		_ASSERT(OverrideClass->IsChildOf(ReturnType));

		UObject* Template = OverrideClass->GetDefaultObject(); // CDO가 아직 생성되지 않았다면 강제로 생성
		EObjectFlags SubobjectFlags = /*Outer->GetMaskedFlags(RF_PropagateToSubObjects) |*/ RF_DefaultSubObject;
		const bool bOwnerTemplateIsNotCDO = ObjectArchetype != nullptr && ObjectArchetype != Outer->GetClass()->GetDefaultObject(false) && !Outer->HasAnyFlags(RF_ClassDefaultObject);
//#if !UE_BUILD_SHIPPING
//		// Guard against constructing the same subobject multiple times.
//		// We only need to check the name as ConstructObject would fail anyway if an object of the same name but different class already existed.
//		if (ConstructedSubobjects.Find(SubobjectFName) != INDEX_NONE)
//		{
//			UE_LOG(LogUObjectGlobals, Fatal, TEXT("Default subobject %s %s already exists for %s."), *OverrideClass->GetName(), *SubobjectFName.ToString(), *Outer->GetFullName());
//		}
//		else
//		{
//			ConstructedSubobjects.Add(SubobjectFName);
//		}
//#endif
		FStaticConstructObjectParameters Params(OverrideClass);
		Params.Outer = Outer;
		Params.Name = SubobjectFName;
		Params.SetFlags = SubobjectFlags;
		Params.SubobjectOverrides = ComponentOverride.SubOverrides;

		// 객체가 CDO가 아닌 템플릿에서 서브오브젝트를 생성하는 경우
		// 새로운 서브오브젝트의 템플릿으로 해당 템플릿의 서브오브젝트를 사용해야 합니다
		if (!bIsTransient && bOwnerTemplateIsNotCDO)
		{
			_ASSERT(false);
			/*UObject* MaybeTemplate = ObjectArchetype->GetDefaultSubobjectByName(SubobjectFName);
			if (MaybeTemplate && Template != MaybeTemplate && MaybeTemplate->IsA(ReturnType))
			{
				Params.Template = MaybeTemplate;
			}*/
		}

		Result = StaticConstructObject_Internal(Params);

		if (Params.Template)
		{
			_ASSERT(false);
			//ComponentInits.Add(Result, Params.Template);
		}
		/*else if (!bIsTransient && Outer->GetArchetype()->IsInBlueprint())
		{
			UObject* MaybeTemplate = Result->GetArchetype();
			if (MaybeTemplate && Template != MaybeTemplate && MaybeTemplate->IsA(ReturnType))
			{
				ComponentInits.Add(Result, MaybeTemplate);
			}
		}*/
		if (Outer->HasAnyFlags(RF_ClassDefaultObject) && Outer->GetClass()->GetSuperClass())
		{
//#if WITH_EDITOR
//			// CDO의 기본 서브오브젝트는 트랜잭셔널이어야 하며, 이를 통해 해당 객체에 가한 변경을 실행 취소/재실행할 수 있습니다.
//			// 현재 이의 예로는 Blueprint Editor에서 원시적으로 정의된 컴포넌트를 편집하는 것이 있습니다.
//			Result->SetFlags(RF_Transactional);
//#endif
			//Outer->GetClass()->AddDefaultSubobject(Result, ReturnType);
		}
		// 죽은 객체의 서브오브젝트를 재활용한 경우 PendingKill 플래그를 해제합니다.
		// @todo: 패키지에서 로드 중인 경우가 아니면 서브오브젝트를 재활용하지 않아야 합니다.
		//Result->ClearGarbage();
	}
	return Result.get();
}

FObjectInitializer::FObjectInitializer(TObjectPtr<UObject>& InObj, const FStaticConstructObjectParameters& StaticConstructParams)
	: SharedObj(InObj)
	, Class(StaticConstructParams.Class)
	, OuterPrivate(StaticConstructParams.Outer)
	, ObjectFlags(StaticConstructParams.SetFlags)
	, Name(StaticConstructParams.Name)
	, ObjectArchetype(StaticConstructParams.Template)
{
	InitializerStack.push_back(this);

	if (ObjectArchetype)
	{
		bShouldInitializePropsFromArchetype = true;
	}
}

FObjectInitializer::~FObjectInitializer()
{
	PostConstructInit();

	InitializerStack.pop_back();
}
//#include "Math/SimpleMath.h"
void FObjectInitializer::InitProperties(UObject* Obj, UClass* DefaultsClass, UObject* DefaultData, bool bCopyTransientsFromClassDefaults)
{
	if (DefaultData)
	{
		type Type = resolve(Hash(DefaultData->GetClass()->ClassName.data()));
		Type.data([&](meta::data Data)
			{
				Data.prop([&](meta::prop p)
					{
						FProperty Prop = p.value().cast<FProperty>();
						void* Dst = Data.get(handle(Type.GetNode(), Obj)).data();
						void* Src = Data.get(handle(Type.GetNode(), DefaultData)).data();
						switch (Prop.PropertyType)
						{
						default:
							if (Prop.PropertySize > 0)
							{
								memcpy(Dst, Src, Prop.PropertySize);
							}
							break;
						/*case T_FVector:
						{
							FVector* _Dest = (FVector*)Dest;
							FVector* _Src = (FVector*)Src;
							*_Dest = *_Src;
							break;
						}
						case T_FRotator:
						{
							FRotator* _Dest = (FRotator*)Dest;
							FRotator* _Src = (FRotator*)Src;
							*_Dest = *_Src;
							break;
						}*/
						}
					});
			});
	}
}

void FObjectInitializer::PostConstructInit()
{
	if (bShouldInitializePropsFromArchetype)
	{
		UClass* BaseClass = /*(bIsCDO && !GIsDuplicatingClassForReinstancing) ? SuperClass :*/ Class;
		if (BaseClass == NULL)
		{
			_ASSERT(Class == UObject::StaticClass());
			BaseClass = Class;
		}

		UObject* Defaults = ObjectArchetype ? ObjectArchetype : BaseClass->GetDefaultObject(false); // we don't create the CDO here if it doesn't already exist
		InitProperties(Obj, BaseClass, Defaults, false/*bCopyTransientsFromClassDefaults*/);
	}

	// InitSubobjectProperties
	{
	}

	SharedObj->PostInitProperties();
}

FString MakeUniqueName(UObject* Outer, const FString& InClassName, FString& InObjectName)
{
	static map<FString, int64> GlobalNameCountMap;

	if (InObjectName.empty())
	{
		InObjectName = InClassName;
	}

	if (!FindObject(InClassName, InObjectName, Outer))
	{
		return InObjectName;
	}

	int64& NewIndex = GlobalNameCountMap[InObjectName];
	FString NewName = InObjectName + TEXT("_") + to_wstring(NewIndex);
	++NewIndex;

	TEnginePtr<UObject> FoundObject;
	do
	{
		if (FoundObject = FindObject(InClassName, NewName, Outer))
		{
			NewName = MakeUniqueName(Outer, InClassName, NewName);
		}
	} while (FoundObject);

	return NewName;
}

CORE_API TObjectPtr<UObject> StaticConstructObject_Internal(FStaticConstructObjectParameters& Params)
{
	UClass* InClass = Params.Class;
	FName& InName = Params.Name;
	
	{
		FString StringName = InName.ToString();
		InName = MakeUniqueName(Params.Outer, InClass->ClassName, StringName);
	}

	TObjectPtr<UObject> Result = NULL;

	InClass->ClassConstructor(FObjectInitializer(Result, Params));

	auto& ObjectVector = ObjectMap[InClass];
	ObjectVector.emplace(InName.ToString(), Result);

	return Result;
}

CORE_API string to_string(const FString& InString)
{
	// wstring -> UTF8
	return wstring_convert<codecvt_utf8<WIDECHAR>>().to_bytes(InString);
}

CORE_API string to_string(FStringView InString)
{
	// wstring -> UTF8
	return wstring_convert<codecvt_utf8<WIDECHAR>>().to_bytes(InString.data());
}

CORE_API FString to_wstring(string_view InString)
{
	// UTF8 -> wstring
	return wstring_convert<codecvt_utf8<WIDECHAR>>().from_bytes(InString.data());
}

CORE_API uint64 operator""_hash(const ANSICHAR * NewString, size_t)
{
	// 문자열을 Hash function에 넣으면 숫자가 나온다
	// 같은 문자열을 넣으면 같은 숫자가 난온다
	// Hash 충돌: 그러나 가끔 다른 문자열을 넣었을때 동일한 숫자가 나올수도 있다.
	static hash<string_view> HashFunction;
	return HashFunction(NewString);
}

CORE_API uint64 operator""_hash(const WIDECHAR * NewString, size_t)
{
	static hash<wstring_view> HashFunction;
	return HashFunction(NewString);
}

CORE_API uint64 Hash(const ANSICHAR* NewString)
{
	static hash<string_view> HashFunction;
	return HashFunction(NewString);
}

CORE_API uint64 Hash(const WIDECHAR* NewString)
{
	static hash<wstring_view> HashFunction;
	return HashFunction(NewString);
}

bool FObjectInitializer::FOverrides::IsLegalOverride(const UClass* DerivedComponentClass, const UClass* BaseComponentClass)
{
	if (DerivedComponentClass && BaseComponentClass && !DerivedComponentClass->IsChildOf(BaseComponentClass))
	{
		return false;
	}
	return true;
}

void FObjectInitializer::FOverrides::Add(FName InComponentName, /*const*/ UClass* InComponentClass)
{
	auto GetSubobjectPath = [InComponentName/*, InFullComponentPath*/]()
		{
			/*if (InFullComponentPath)
			{
				FString SubobjectPath;
				for (FName SubobjectName : *InFullComponentPath)
				{
					SubobjectPath += (SubobjectPath.IsEmpty() ? TEXT("") : TEXT("."));
					SubobjectPath += SubobjectName.ToString();
				}
				return SubobjectPath;
			}*/

			return InComponentName.ToString();
		};

	const int32 Index = Find(InComponentName);
	if (Index == INDEX_NONE)
	{
		FOverride& Override = Overrides.emplace_back(FOverride(InComponentName));
		Override.ComponentClass = InComponentClass;
		Override.bDoNotCreate = (InComponentClass == nullptr);
	}
	else if (InComponentClass)
	{
		if (Overrides[Index].ComponentClass)
		{
			// 기본 클래스가 오버라이드를 요청하는 경우, 기존의 오버라이드(우리가 사용할 오버라이드)는 상속되어야 합니다.
			if (!IsLegalOverride(Overrides[Index].ComponentClass, InComponentClass))
			{
				E_LOG(Error, TEXT("{}는 컴포넌트 {}에 대한 합법적인 오버라이드가 아닙니다. {}에서 파생되지 않았기 때문입니다. 컴포넌트를 생성할 때 {}를 사용할 것입니다."),
					Overrides[Index].ComponentClass->GetName(), GetSubobjectPath(), InComponentClass->GetName(), InComponentClass->GetName());

				Overrides[Index].ComponentClass = InComponentClass;
			}
		}
		else
		{
			// 기존에 기록된 컴포넌트 클래스가 null인 경우 서브 오버라이드가 있을 수 있으므로 여전히 클래스를 사용하려고 합니다.
			// 또는 생성하지 않도록 표시될 수 있지만 기본 클래스가 선택적이 아닌 것으로 생성할 수 있으므로 클래스를 기록하려고 합니다.
			Overrides[Index].ComponentClass = InComponentClass;
		}

	}
	else
	{
		// 기존의 오버라이드가 있지만 상위 클래스가 이를 DoNotCreate로 표시하는 경우 경고합니다.
		// 오류를 보고하더라도, 컴포넌트가 선택 사항이 아닌 것으로 생성되면 이러한 오버라이드가 여전히 사용될 수 있습니다.
		if (Overrides[Index].ComponentClass)
		{
			E_LOG(Error, TEXT("{}는 컴포넌트 {}에 대한 합법적인 오버라이드가 아닙니다. 상위 클래스가 이를 생성하지 않도록 표시했기 때문입니다."),
				Overrides[Index].ComponentClass->GetName(), GetSubobjectPath());
		}
		if (Overrides[Index].SubOverrides)
		{
			E_LOG(Error, TEXT("컴포넌트 {}는 기록된 중첩 서브오브젝트 오버라이드를 가지고 있지만, 상위 클래스가 이를 생성하지 않도록 표시했기 때문에 생성되지 않습니다."),
				GetSubobjectPath());
		}

		Overrides[Index].bDoNotCreate = true;
	}
}

FObjectInitializer::FOverrides::FOverrideDetails FObjectInitializer::FOverrides::Get(FName InComponentName, const UClass* ReturnType, UClass* ClassToConstructByDefault, bool bOptional) const
{
	FOverrideDetails Result;

	const int32 Index = Find(InComponentName);
	if (Index == INDEX_NONE)
	{
		Result.Class = ClassToConstructByDefault; // 오버라이드가 없으므로 기본 클래스에서 원하는 대로 수행
		Result.SubOverrides = nullptr;
	}
	else if (Overrides[Index].bDoNotCreate && bOptional)
	{
		Result.Class = nullptr;   // 오버라이드는 nullptr, 즉 "이 컴포넌트를 생성하지 않음"을 의미
		Result.SubOverrides = nullptr; // 그리고 이 컴포넌트를 생성하지 않으면 서브 오버라이드도 필요하지 않음
	}
	else if (Overrides[Index].ComponentClass)
	{
		if (IsLegalOverride(Overrides[Index].ComponentClass, ReturnType)) // 기본 클래스가 T를 요청하는 경우, 기존 오버라이드(사용할 예정인)는 파생되어야 함
		{
			Result.Class = Overrides[Index].ComponentClass; // 오버라이드는 적절한 클래스이므로 사용

			if (Overrides[Index].bDoNotCreate)
			{
				E_LOG(Error, TEXT("필수로 지정된 {}에 대해 DoNotCreateDefaultSubobject를 무시했습니다. {}를 생성 중입니다."), InComponentName.ToString(), Result.Class->GetName());
			}
		}
		else
		{
			if (Overrides[Index].bDoNotCreate)
			{
				E_LOG(Error, TEXT("필수로 지정된 {}에 대해 DoNotCreateDefaultSubobject를 무시했습니다. {}를 생성 중입니다."), InComponentName.ToString(), ClassToConstructByDefault->GetName());
			}
			//E_LOG(Error, TEXT("{}는 {} 컴포넌트에 대한 적법한 오버라이드가 아니므로 {}를 사용하여 컴포넌트를 생성 중입니다."),
			//  Overrides[Index].ComponentClass->GetFullName(), InComponentName.ToString(), ReturnType->GetFullName(), ClassToConstructByDefault->GetFullName());

			Result.Class = ClassToConstructByDefault;
		}
		Result.SubOverrides = Overrides[Index].SubOverrides.get();
	}
	else
	{
		if (Overrides[Index].bDoNotCreate)
		{
			E_LOG(Error, TEXT("필수로 지정된 {}에 대해 DoNotCreateDefaultSubobject를 무시했습니다. {}를 생성 중입니다."), InComponentName.ToString(), ClassToConstructByDefault->GetName());
		}

		Result.Class = ClassToConstructByDefault; // 서브 오버라이드만 오버라이드되었으므로 기본 클래스의 설정을 사용
		Result.SubOverrides = Overrides[Index].SubOverrides.get();
	}

	return Result;
}
