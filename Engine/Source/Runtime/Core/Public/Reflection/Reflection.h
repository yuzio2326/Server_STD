#pragma once
#include "Windows/WindowsPlatform.h"
#include "Misc/EnumClassFlags.h"
#include "meta.hpp"
#include "factory.hpp"
#include "policy.hpp"

using namespace meta;

enum EPropertyType : uint8
{
	T_NONE,
	//T_POINTER,
	//T_SHARED_PTR,
	T_ENGINE_PTR,
	//T_WEAK_PTR,
	T_BOOL,
	T_UINT8,
	T_INT,
	T_FLOAT,
	T_FVECTOR,
	T_FROTATOR,
};

enum EPropertyFlags : uint8
{
	NoFlags = 0x00000000,

	/// Indicates that this property can be edited by property windows in the editor
	EditAnywhere = 0x00000001,
	VisibleAnywhere = 0x00000002,
	//AAA	= 0x00000004,
};

struct FProperty
{
	EPropertyFlags PropertyFlags = EPropertyFlags::NoFlags;
	EPropertyType PropertyType = EPropertyType::T_NONE; // 변수 타입
	std::string Name;		// 해당 Property의 이름(변수 이름)
	FString ClassName;		// 해당 Property가 Class인 경우에만 지정 됩니다.
	bool bBaseType = false; // int 등과 같은 기본 자료는 size 구해서 copy 할 목적
	size_t PropertySize = 0;// 변수 타입 크기(우선은 기본 자료형만)
};

// Detail 패널에서 항목 편집을 위해 필요한 정보
struct FPropertyInfo
{
	FPropertyInfo(const FProperty& InProperty, void* InPropertyAddress)
		: Property(InProperty), PropertyType(InProperty.PropertyType), PropertySize(InProperty.PropertySize)
		, PropertyAddress(InPropertyAddress) {}

	FProperty Property;
	EPropertyType PropertyType = EPropertyType::T_NONE; // 변수 타입
	size_t PropertySize = 0;// 변수 타입 크기(우선은 기본 자료형만)

	void* PropertyAddress = nullptr; // 실제 Property의 주소
};