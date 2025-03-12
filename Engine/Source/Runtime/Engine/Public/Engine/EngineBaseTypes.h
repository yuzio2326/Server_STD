#pragma once

#include "CoreMinimal.h"

/** URL 구조체입니다. */
//USTRUCT()
struct FURL
{
    //GENERATED_USTRUCT_BODY()

    // 프로토콜, 예: "unreal" 또는 "http".
    //UPROPERTY()
    FString Protocol;

    // 선택적 호스트 이름, 예: "204.157.115.40" 또는 "unreal.epicgames.com", 로컬인 경우 빈 문자열입니다.
    //UPROPERTY()
    FString Host;

    // 선택적 호스트 포트입니다.
    //UPROPERTY()
    int32 Port;

    //UPROPERTY()
    int32 Valid;

    // 맵 이름, 예: "SkyCity", 기본값은 "Entry".
    //UPROPERTY()
    FString Map;

    // 클라이언트가 맵을 소유하지 않을 경우 맵을 다운로드할 장소
    //UPROPERTY()
    FString RedirectURL;

    // 옵션들입니다.
    //UPROPERTY()
    TArray<FString> Op;

    // 들어갈 포털, 기본값은 빈 문자열입니다.
    //UPROPERTY()
    FString Portal;

    // Statics.
    //static ENGINE_API FUrlConfig UrlConfig;
    //static ENGINE_API bool bDefaultsInitialized;

    ///**
    // * 기본 생성을 방지합니다.
    // */
    //explicit FURL(ENoInit) {}

    ///**
    // * 옵셔널 파일 이름으로부터 로컬 URL을 생성합니다.
    // */
    //ENGINE_API FURL(const TCHAR* Filename = nullptr);

    ///**
    // * 텍스트 URL과 선택적 상대 기준으로 URL을 생성합니다.
    // */
    //ENGINE_API FURL(FURL* Base, const TCHAR* TextURL, ETravelType Type);

    //static ENGINE_API void StaticInit();
    //static ENGINE_API void StaticExit();

    ///**
    // * Static: 지정된 문자열에서 URL 특수 문자를 제거합니다.
    // *
    // * @param Str 필터링할 문자열
    // */
    //static ENGINE_API void FilterURLString(FString& Str);

    ///**
    // * 이 URL이 내부 객체에 해당하는지 여부를 반환합니다. 예를 들어, 이 애플리케이션이 로컬 또는 네트워크상의 Unreal 레벨에 연결할 수 있는 경우입니다. 이가 false이면, URL은 인터넷 익스플로러와 같은 원격 애플리케이션이 실행할 수 있는 객체를 가리킵니다.
    // */
    //ENGINE_API bool IsInternal() const;

    ///**
    // * 이 URL이 이 로컬 프로세스에서 내부 객체에 해당하는지 여부를 반환합니다. 이 경우 인터넷 사용이 필요하지 않습니다.
    // */
    //ENGINE_API bool IsLocalInternal() const;

    ///**
    // * URL이 옵션 문자열을 포함하는지 테스트합니다.
    // */
    //ENGINE_API bool HasOption(const TCHAR* Test) const;

    ///**
    // * 옵션의 값을 반환합니다.
    // *
    // * @param Match 가져올 옵션의 이름
    // * @param Default 옵션이 발견되지 않을 경우 반환할 값
    // *
    // * @return 옵션의 값 또는 Default 값
    // */
    //ENGINE_API const TCHAR* GetOption(const TCHAR* Match, const TCHAR* Default) const;

    ///**
    // * URL을 구성 파일에서 로드합니다.
    // */
    //ENGINE_API void LoadURLConfig(const TCHAR* Section, const FString& Filename = GGameIni);

    ///**
    // * URL을 구성 파일에 저장합니다.
    // */
    //ENGINE_API void SaveURLConfig(const TCHAR* Section, const TCHAR* Item, const FString& Filename = GGameIni) const;

    ///**
    // * 고유한 옵션을 URL에 추가하며, 기존 옵션을 대체합니다.
    // */
    //ENGINE_API void AddOption(const TCHAR* Str);

    ///**
    // * URL에서 옵션을 제거합니다.
    // */
    //ENGINE_API void RemoveOption(const TCHAR* Key, const TCHAR* Section = nullptr, const FString& Filename = GGameIni);

    ///**
    // * 이 URL을 텍스트로 변환합니다.
    // */
    //ENGINE_API FString ToString(bool FullyQualified = 0) const;

    ///**
    // * Host와 Port 값을 표준을 준수하는 문자열로 준비합니다.
    // */
    //ENGINE_API FString GetHostPortString() const;

    ///**
    // * FURL을 아카이브로 직렬화하거나 역직렬화합니다.
    // */
    //ENGINE_API friend FArchive& operator<<(FArchive& Ar, FURL& U);

    ///**
    // * 두 URL이 동일한 것을 가리킬지를 비교합니다.
    // */
    //ENGINE_API bool operator==(const FURL& Other) const;
};
