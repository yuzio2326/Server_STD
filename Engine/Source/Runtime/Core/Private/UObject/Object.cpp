#include "UObject/Object.h"
#include "UObject/Class.h"
#include "UObject/UObjectGlobals.h"
#include "Logging/Logger.h"
#include "CoreMinimal.h"

UClass* UObjectRegisterEngineClass = TGetPrivateStaticClassBody<UObject>(
	TEXT("UObject"), InternalConstructor<UObject>, nullptr, nullptr);

void UObject::Serialize(FArchive& Ar)
{
	struct FPropertyData
	{
		FPropertyData() = default;
		FPropertyData(meta::data& InData, meta::prop& InProp) : Data(InData), Prop(InProp) {}
		meta::data Data;
		meta::prop Prop;
	};
	map<string, FPropertyData> PropertyDatas;
	type Type = resolve(Hash(GetClass()->ClassName.data()));
	Type.data([&](meta::data Data)
		{
			Data.prop([&](meta::prop p)
				{
					FProperty Prop = p.value().cast<FProperty>();
					if (Ar.IsLoading())
					{
						PropertyDatas.emplace(Prop.Name, FPropertyData(Data, p));
					}
					else
					{
						// CDO 값과 같으면 저장에서 생략한다
						bool bDefaultValue = false;
						if (Prop.bBaseType)
						{
							void* Value = (void*)Data.get(handle(Type.GetNode(), this)).data();
							void* CDOValue = (void*)Data.get(handle(Type.GetNode(), GetClass()->GetDefaultObject())).data();
							if (0 == memcmp(Value, CDOValue, Prop.PropertySize))
							{
								bDefaultValue = true;
							}
						}
						else
						{
							switch (Prop.PropertyType)
							{
							case EPropertyType::T_FROTATOR:
							{
								FRotator* Value = (FRotator*)Data.get(handle(Type.GetNode(), this)).data();
								FRotator* CDOValue = (FRotator*)Data.get(handle(Type.GetNode(), GetClass()->GetDefaultObject())).data();
								if (*CDOValue == *Value) { bDefaultValue = true; }
								break;
							}
							case EPropertyType::T_FVECTOR:
							{
								FVector* Value = (FVector*)Data.get(handle(Type.GetNode(), this)).data();
								FVector* CDOValue = (FVector*)Data.get(handle(Type.GetNode(), GetClass()->GetDefaultObject())).data();
								if (*CDOValue == *Value) { bDefaultValue = true; }
								break;
							}
							case EPropertyType::T_ENGINE_PTR:
							{
								TEnginePtr<UObject>* Value = (TEnginePtr<UObject>*)Data.get(handle(Type.GetNode(), this)).data();
								TEnginePtr<UObject>* CDOValue = (TEnginePtr<UObject>*)Data.get(handle(Type.GetNode(), GetClass()->GetDefaultObject())).data();
								if (*CDOValue == *Value) { bDefaultValue = true; }
								break;
							}
							default:
								bDefaultValue = true;
								break;
							}
						}
						if (!bDefaultValue)
						{
							PropertyDatas.emplace(Prop.Name, FPropertyData(Data, p));
						}
					}
				}
			);
		}
	);

	if (Ar.IsSaving())
	{
		FString ObjectName = GetName();
		Ar << ObjectName;

		uint64 PropSize = PropertyDatas.size();
		Ar << PropSize;
		for (auto& It : PropertyDatas)
		{
			FProperty Prop = It.second.Prop.value().cast<FProperty>();
			string& PropName = Prop.Name;
			Ar << PropName;

			switch (Prop.PropertyType)
			{
			case EPropertyType::T_BOOL:
			{
				bool* Value = (bool*)PropertyDatas[PropName].Data.get(handle(Type.GetNode(), this)).data();
				Ar << *Value;
				break;
			}
			case EPropertyType::T_INT:
			{
				int* Value = (int*)PropertyDatas[PropName].Data.get(handle(Type.GetNode(), this)).data();
				Ar << *Value;
				break;
			}
			case EPropertyType::T_FLOAT:
			{
				float* Value = (float*)PropertyDatas[PropName].Data.get(handle(Type.GetNode(), this)).data();
				Ar << *Value;
				break;
			}
			case EPropertyType::T_FROTATOR:
			{
				FRotator* Value = (FRotator*)PropertyDatas[PropName].Data.get(handle(Type.GetNode(), this)).data();
				Ar << *Value;
				break;
			}
			case EPropertyType::T_FVECTOR:
			{
				FVector* Value = (FVector*)PropertyDatas[PropName].Data.get(handle(Type.GetNode(), this)).data();
				Ar << *Value;
				break;
			}
			case EPropertyType::T_ENGINE_PTR:
			{
				TEnginePtr<UObject>* Value = (TEnginePtr<UObject>*)PropertyDatas[PropName].Data.get(handle(Type.GetNode(), this)).data();
				Ar << *Value;
				break;
			}
			}
		}
	}
	else if(Ar.IsLoading())
	{
		if (!Ar.IsPersistent())
		{
			FString ObjectName;
			Ar << ObjectName;
			// 불러온 이름으로 덮어쓸껀지 고민
		}
		uint64 PropSize = 0;
		Ar << PropSize;
		for (uint64 i = 0; i < PropSize; ++i)
		{
			string PropName;
			Ar << PropName;

			if (PropertyDatas.contains(PropName))
			{
				FProperty Prop = PropertyDatas[PropName].Prop.value().cast<FProperty>();
				switch (Prop.PropertyType)
				{
				case EPropertyType::T_BOOL:
				{
					bool* Value = (bool*)PropertyDatas[PropName].Data.get(handle(Type.GetNode(), this)).data();
					Ar << *Value;
					break;
				}
				case EPropertyType::T_INT:
				{
					int* Value = (int*)PropertyDatas[PropName].Data.get(handle(Type.GetNode(), this)).data();
					Ar << *Value;
					break;
				}
				case EPropertyType::T_FLOAT:
				{
					float* Value = (float*)PropertyDatas[PropName].Data.get(handle(Type.GetNode(), this)).data();
					Ar << *Value;
					break;
				}
				case EPropertyType::T_FROTATOR:
				{
					FRotator* Value = (FRotator*)PropertyDatas[PropName].Data.get(handle(Type.GetNode(), this)).data();
					Ar << *Value;
					break;
				}
				case EPropertyType::T_FVECTOR:
				{
					FVector* Value = (FVector*)PropertyDatas[PropName].Data.get(handle(Type.GetNode(), this)).data();
					Ar << *Value;
					break;
				}
				case EPropertyType::T_ENGINE_PTR:
				{
					TEnginePtr<UObject>* Value = (TEnginePtr<UObject>*)PropertyDatas[PropName].Data.get(handle(Type.GetNode(), this)).data();
					Ar << *Value;
					break;
				}
				}
				OnPropertyChanged(Prop);
			}
		}
	}
}

UObject* UObject::CreateDefaultSubobject(FName SubobjectFName, UClass* ReturnType, UClass* ClassToCreateByDefault, bool bIsRequired, bool bIsTransient)
{
	if (FObjectInitializer::InitializerStack.empty())
	{
		E_LOG(Fatal, TEXT("FObjectInitializer::InitializerStack is empty."));
		return nullptr;
	}
	FObjectInitializer* CurrentInitializer = FObjectInitializer::InitializerStack[FObjectInitializer::InitializerStack.size() - 1];
	if (!CurrentInitializer)
	{
		E_LOG(Fatal, TEXT("No object initializer found during construction."));
		return nullptr;
	}
	if (CurrentInitializer->Obj != this)
	{
		E_LOG(Fatal, TEXT("Using incorrect object initializer."));
		return nullptr;
	}

	return CurrentInitializer->CreateDefaultSubobject(this, SubobjectFName, ReturnType, ClassToCreateByDefault, bIsRequired, bIsTransient);
}

void UObject::PostInitProperties()
{
}

void UObject::__DefaultConstructor(const FObjectInitializer& X)
{
	new((EInternal*)X.GetObj())UObject;
}

UClass* UObject::StaticClass()
{
	return UObjectRegisterEngineClass;
}

UWorld* UObject::GetWorld() const
{
	if (UObject* Outer = GetOuter())
	{
		return Outer->GetWorld();
	}

	return nullptr;
}