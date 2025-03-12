#pragma once
#include "CoreTypes.h"
#include <boost/serialization/nvp.hpp>
#include "Math/SimpleMath.h"
#include "UObject/EnginePtr.h"

namespace boost {
	namespace serialization { template<class T> class nvp; }
	namespace archive
	{
		class text_oarchive;
		class text_iarchive;
	}
}
struct FRotator;
class UObject;
class CORE_API FArchive
{
public:
	enum class EMode { Save, Load };

	FArchive(boost::archive::text_oarchive& OutputArchive);
	FArchive(boost::archive::text_oarchive& OutputArchive, 
		map<UObject*, TObjectPtr<UObject>>& InDuplicatedObjects, TObjectPtr<UObject> SourceObject, TObjectPtr<UObject> DestObject);
	FArchive(boost::archive::text_iarchive& InputArchive);
	FArchive(boost::archive::text_iarchive& InputArchive, 
		map<UObject*, TObjectPtr<UObject>>& InDuplicatedObjects);

	FArchive& operator<<(bool& InOutData);
	FArchive& operator<<(int32& InOutData);
	FArchive& operator<<(float& InOutData);
	FArchive& operator<<(FRotator& InOutData);
	FArchive& operator<<(FVector& InOutData);
	FArchive& operator<<(TEnginePtr<UObject>& InOutData);
	FArchive& operator<<(uint64& InOutData);
	FArchive& operator<<(FString& InString);
	FArchive& operator<<(string& InString);
	FArchive& operator<<(UObject*& InOutObjectPtr);

	template<typename T>
	FArchive& operator<<(TObjectPtr<T>& InOutData)
	{
		static_assert(is_base_of<UObject, T>::value, "T must be derived from UObject");

		UObject* Object = InOutData.get();
		*this << Object;

		if (IsLoading())
		{
			shared_ptr<UObject> ObjectData;
			Convert(ObjectData, Object);
			InOutData = dynamic_pointer_cast<T>(ObjectData);
		}
		return *this;
	}

	inline bool IsSaving() const { return Mode == EMode::Save; }
	inline bool IsLoading() const { return Mode == EMode::Load; }

	/**
	 * 이 아카이브가 지속적인 저장소를 위한 데이터를 저장하거나 로드하고 있으며 일시적인 데이터를 건너뛰어야 하는 경우 true를 반환합니다.
	 * 복제와 같은 작업을 위한 일부 중간 아카이브에서도, 결국 지속적인 저장소로 향하게 되는 경우에도 true를 반환합니다.
	 */
	// 우리는 디스크 Save Load일때는 true, 메모리 상에서 Editor -> PIE 복제의 경우 false
	inline bool IsPersistent() const { return !DuplicatedObjectAnnotation; }

	/**
	 * 새로운 복제본을 DuplicatedObjects 맵과 UnserializedObjects 목록에 추가합니다.
	 *
	 * @param   SourceObject    원본 객체
	 * @param   DuplicateObject 객체의 복사본
	 */
	void AddDuplicate(TObjectPtr<UObject> SourceObject, TObjectPtr<UObject> DuplicateObject);

	/**
	 * 주어진 객체의 복제본에 대한 포인터를 반환하며, 필요에 따라 복제 객체를 생성합니다.
	 *
	 * @param   Object  복제본을 찾아야 하는 객체
	 * @param   bCreateIfMissing    복제본이 없는 경우 복제 객체를 생성할지 여부
	 *
	 * @return  지정된 객체의 복제본에 대한 포인터
	 */
	UObject* GetDuplicatedObject(UObject* Object, bool bCreateIfMissing = true);

	std::stack<TObjectPtr<UObject>>	UnserializedObjects;

protected:
	void Convert(TObjectPtr<UObject>& InOutSharedObject, UObject* InObject);

private:
	EMode Mode;
	map<UObject*, TObjectPtr<UObject>>* DuplicatedObjectAnnotation = nullptr; // Key: Src, Val: Dup
	boost::archive::text_oarchive* SaveArchive = nullptr;
	boost::archive::text_iarchive* LoadArchive = nullptr;
};