#pragma once
#include "UObject/UObjectMacros.h"
#include "HAL/Platform.h"
#include <string>
#include "Misc/CoreMiscDefines.h"
using FStringView = std::wstring_view;
using FString = std::wstring;
enum class EName : uint32
{
	MaxHardcodedNameIndex,
};

/**
 * 세계에 공개된 이름입니다. 이름은 고유 문자열 테이블의 인덱스와 인스턴스 번호의 조합으로 저장됩니다.
 * 이름은 대소문자를 구분하지 않지만, (WITH_CASE_PRESERVING_NAME이 1인 경우) 대소문자를 보존합니다.
 */
// (우리는 대소문자 구분 하고 있음)
class CORE_API FName
{
public:
	FName() {}
	FName(ENoInit);
	FName(EName InHashCode);
	FName(FStringView InString);
	FName(const FString& InString);
	FName(const WIDECHAR* InString);

	/**
	 * Converts an FName to a readable format
	 *
	 * @return String representation of the name
	 */
	FString ToString() const;

	/**
	 * Converts an FName to a readable format, in place
	 *
	 * @param Out String to fill with the string representation of the name
	 */
	void ToString(FString& Out) const;

	bool operator<(const FName& Other) const { return HashCode < Other.HashCode; }
	bool operator==(const FName& Other) const { return HashCode == Other.HashCode; }

private:
	uint64 HashCode;
};

const static inline FName NAME_None = FName(EName(0));