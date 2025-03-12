#include "Serialization/Archive.h"
#include "UObject/UObjectGlobals.h"
#include "UObject/Class.h"
#include "UObject/Object.h"
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include "Math/Rotator.h"

FArchive::FArchive(boost::archive::text_oarchive& OutputArchive)
	: Mode(EMode::Save), SaveArchive(&OutputArchive)
{
}

FArchive::FArchive(boost::archive::text_oarchive& OutputArchive, map<UObject*, TObjectPtr<UObject>>& InDuplicatedObjects, 
	TObjectPtr<UObject> SourceObject, TObjectPtr<UObject> DestObject)
	: Mode(EMode::Save), SaveArchive(&OutputArchive), DuplicatedObjectAnnotation(&InDuplicatedObjects)
{
	AddDuplicate(SourceObject, DestObject);
}

FArchive::FArchive(boost::archive::text_iarchive& InputArchive)
	: Mode(EMode::Load), LoadArchive(&InputArchive)
{
}

FArchive::FArchive(boost::archive::text_iarchive& InputArchive, map<UObject*, TObjectPtr<UObject>>& InDuplicatedObjects)
	: Mode(EMode::Load), LoadArchive(&InputArchive), DuplicatedObjectAnnotation(&InDuplicatedObjects)
{
}

FArchive& FArchive::operator<<(uint64& InOutData)
{
	if (IsSaving())
	{
		(*SaveArchive) & InOutData;
	}
	else
	{
		(*LoadArchive) & InOutData;
	}
	return *this;
}

FArchive& FArchive::operator<<(bool& InOutData)
{
	if (IsSaving())
	{
		(*SaveArchive)& InOutData;
	}
	else
	{
		(*LoadArchive)& InOutData;
	}
	return *this;
}

FArchive& FArchive::operator<<(int32& InOutData)
{
	if (IsSaving())
	{
		(*SaveArchive)& InOutData;
	}
	else
	{
		(*LoadArchive)& InOutData;
	}
	return *this;
}

FArchive& FArchive::operator<<(float& InOutData)
{
	if (IsSaving())
	{
		(*SaveArchive)& InOutData;
	}
	else
	{
		(*LoadArchive)& InOutData;
	}
	return *this;
}

FArchive& FArchive::operator<<(FRotator& InOutData)
{
	if (IsSaving())
	{
		(*SaveArchive)& InOutData.Pitch;
		(*SaveArchive)& InOutData.Yaw;
		(*SaveArchive)& InOutData.Roll;
	}
	else
	{
		(*LoadArchive)& InOutData.Pitch;
		(*LoadArchive)& InOutData.Yaw;
		(*LoadArchive)& InOutData.Roll;
	}
	return *this;
}

FArchive& FArchive::operator<<(FVector& InOutData)
{
	if (IsSaving())
	{
		(*SaveArchive)& InOutData.x;
		(*SaveArchive)& InOutData.y;
		(*SaveArchive)& InOutData.z;
	}
	else
	{
		(*LoadArchive)& InOutData.x;
		(*LoadArchive)& InOutData.y;
		(*LoadArchive)& InOutData.z;
	}
	return *this;
}

FArchive& FArchive::operator<<(TEnginePtr<UObject>& InOutData)
{
	FString ClassName, ObjectName;

	if (IsSaving())
	{
		ClassName = InOutData->GetClass()->ClassName;
		ObjectName = InOutData->GetName();
	}

	*this << ClassName;
	*this << ObjectName;

	if (IsLoading())
	{
		InOutData = FindObject(ClassName, ObjectName);
	}
	return *this;
}

FArchive& FArchive::operator<<(FString& InString)
{
	string String;
	if (IsSaving())
	{
		String = TCHAR_TO_ANSI(InString);
		(*SaveArchive) & String;
	}
	else
	{
		(*LoadArchive) & String;
		InString = ANSI_TO_TCHAR(String);
	}
	return *this;
}

FArchive& FArchive::operator<<(string& InString)
{
	if (IsSaving())
	{
		(*SaveArchive)& InString;
	}
	else
	{
		(*LoadArchive)& InString;
	}
	return *this;
}

FArchive& FArchive::operator<<(UObject*& InOutObjectPtr)
{
	intptr_t Pointer = (intptr_t)InOutObjectPtr;
	if (IsSaving())
	{
		GetDuplicatedObject(InOutObjectPtr);
		(*SaveArchive)& Pointer;
	}
	else
	{
		(*LoadArchive)& Pointer;
		UObject* SourceObject = (UObject*)Pointer;
		TObjectPtr<UObject> DuplicatedObject = (*DuplicatedObjectAnnotation)[SourceObject];
		InOutObjectPtr = DuplicatedObject.get();
	}
	return *this;
}

void FArchive::AddDuplicate(TObjectPtr<UObject> SourceObject, TObjectPtr<UObject> DuplicateObject)
{
	// 기존의 객체 복제본이 있는지 확인합니다. 발견되면 새로운 객체를 생성하는 대신 해당 복제본을 사용합니다.
	if (!DuplicatedObjectAnnotation->contains(SourceObject.get()))
	{
		DuplicatedObjectAnnotation->emplace(SourceObject.get(), DuplicateObject);
	}
	else
	{
		_ASSERT(false);
	}

	UnserializedObjects.push(SourceObject);
}

UObject* FArchive::GetDuplicatedObject(UObject* Object, bool bCreateIfMissing)
{
	TObjectPtr<UObject> Result;
	if (Object)
	{
		if (DuplicatedObjectAnnotation->contains(Object))
		{
			Result = (*DuplicatedObjectAnnotation)[Object];
		}
		else if (bCreateIfMissing)
		{
			UObject* DupOuter = GetDuplicatedObject(Object->GetOuter());
			if (DupOuter != nullptr)
			{
				FStaticConstructObjectParameters Param(Object->GetClass());
				Param.Outer = DupOuter;
				Param.Name = Object->GetName();
				Param.SetFlags = Object->GetFlags();
				Param.Template = Object->GetClass()->GetDefaultObject();
				Result = StaticConstructObject_Internal(Param);

				AddDuplicate(Object->As<UObject>(), Result);
			}
		}
	}

	return Result.get();
}

void FArchive::Convert(TObjectPtr<UObject>& InOutSharedObject, UObject* InObject)
{
	InOutSharedObject = InObject->As<UObject>();
}
