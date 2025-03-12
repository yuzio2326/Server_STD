#include "UObject/NameTypes.h"
#include "UObject/UObjectGlobals.h"
#include "CoreTypes.h"
//#include "Logging/Logger.h"

unordered_map<uint64, FString>& GetNamePool()
{
	static unordered_map<uint64, FString> NamePool;
	return NamePool;
}

#pragma warning(disable:26495)
FName::FName(ENoInit)
{
}
#pragma warning(default:26495)

FName::FName(EName InHashCode)
	: HashCode((uint64)InHashCode)
{
}

FName::FName(FStringView InString)
{
	FString String = InString.data();
	HashCode = Hash(String.data());
	GetNamePool()[HashCode] = String;
}

FName::FName(const FString& InString)
{
	HashCode = Hash(InString.data());
	GetNamePool()[HashCode] = InString;
}

FName::FName(const WIDECHAR* InString)
	:FName(FStringView(InString))
{
}

FString FName::ToString() const
{
	FString Out;
	ToString(Out);
	return Out;
}

void FName::ToString(FString& Out) const
{
	if (HashCode == 0)
	{
		//E_LOG(Error, TEXT("HashCode == 0"));
		return;
	}

	Out = GetNamePool()[HashCode];
}
