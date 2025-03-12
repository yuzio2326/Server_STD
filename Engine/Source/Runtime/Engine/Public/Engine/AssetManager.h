#pragma once
#include "CoreMinimal.h"

/**
 * PrimaryAssets를 로드하고 언로드하며 게임별 자산 참조를 유지하는 책임이 있는 싱글톤 UObject입니다.
 * 게임은 이 클래스를 오버라이드하고 클래스 참조를 변경해야 합니다.
 */
// 우리는 Mesh 등을 로딩하기 위해서 사용
class FAssetManager
{
public:
	/** 현재 AssetManager 객체를 반환합니다 */
	static ENGINE_API FAssetManager* Get(const bool bDestroy = false);

public:
	template<typename T>
	TEnginePtr<T> LoadAsset(const FString& InFilePath, const TCHAR* Params = nullptr)
	{
		return Cast<T>(LoadAsset(typeid(T), InFilePath, Params));
	}

protected:
	TEnginePtr<UObject> LoadAsset(const type_info& InAssetType, const FString& InFilePath, const TCHAR* Params);
	// void UnloadAsset();

protected:
	unordered_map<FHashKey, unordered_map<FString, TObjectPtr<UObject>>> LoadedAssets;
};