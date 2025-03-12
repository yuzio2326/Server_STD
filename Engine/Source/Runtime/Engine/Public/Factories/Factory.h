#pragma once
#include "CoreMinimal.h"
#include "Factory.generated.h"

/**
 * 모든 팩토리의 기본 클래스
 * 새로운 객체를 생성하고 가져오는 객체.
 *
 */
UCLASS()
class UFactory : public UObject
{
	GENERATED_BODY()

public:
	virtual bool FactoryCanImport(const FString& Filename) { return false; }
	virtual TObjectPtr<UObject> FactoryCreateFile(const FName InName, const FString& InFileName, const TCHAR* Params) 
	{
		_ASSERT(false);
		return nullptr;
	}
};