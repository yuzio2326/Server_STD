#pragma once
#include "Misc/EnumClassFlags.h"

enum EInternal { EC_InternalUseOnlyConstructor };

enum EObjectFlags
{
	RF_NoFlags = 0x00000000,

	RF_ClassDefaultObject = 0x00000010,
	RF_Class = 0x00000020,
	RF_Transient = 0x00000040,	///< Don't save object.

	RF_DefaultSubObject = 0x00040000,	///< 클래스 생성자에서 생성된 서브 객체 템플릿과 그 템플릿에서 생성된 모든 인스턴스에 플래그를 지정합니다.

}; 
ENUM_CLASS_FLAGS(EObjectFlags);

#define DEFINE_DEFAULT_CONSTRUCTOR_CALL(TClass) \
	static void __DefaultConstructor(const FObjectInitializer& X) { new((EInternal*)X.GetObj())TClass; }

#define DEFINE_DEFAULT_OBJECT_INITIALIZER_CONSTRUCTOR_CALL(TClass) \
	static void __DefaultConstructor(const FObjectInitializer& X) { new((EInternal*)X.GetObj())TClass(X); }

#define GENERATED_BODY()
#define UCLASS(...)
#define UENUM(...)
#define UPROPERTY(...)
//#define UFUNCTION()

// DLL export and import definitions
#if WITH_EDITOR
#define DLLEXPORT __declspec(dllexport)
#define DLLIMPORT __declspec(dllimport)
#else
#define DLLEXPORT
#define DLLIMPORT
#endif