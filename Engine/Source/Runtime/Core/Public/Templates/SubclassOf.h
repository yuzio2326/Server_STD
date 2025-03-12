#pragma once
#include "UObject/Class.h"

/**
 * Returns the same type passed to it.  This is useful in a few cases, but mainly for inhibiting template argument deduction in function arguments, e.g.:
 *
 * template <typename T>
 * void Func1(T Val); // Can be called like Func(123) or Func<int>(123);
 *
 * template <typename T>
 * void Func2(typename TIdentity<T>::Type Val); // Must be called like Func<int>(123)
 *
 * Equivalent to C++20's std::type_identity.
 */
template <typename T>
struct TIdentity
{
	using Type = T;
	using type = T;
};

template <typename T>
using TIdentity_T = typename TIdentity<T>::Type;

/**
 * Uses implicit conversion to create an instance of a specific type.
 * Useful to make things clearer or circumvent unintended type deduction in templates.
 * Safer than C casts and static_casts, e.g. does not allow down-casts
 *
 * @param Obj  The object (usually pointer or reference) to convert.
 *
 * @return The object converted to the specified type.
 */
template <typename T>
FORCEINLINE T ImplicitConv(typename TIdentity<T>::Type Obj)
{
	return Obj;
}

template <typename T>
class TSubclassOf;

template <typename T>
struct TIsTSubclassOf
{
	enum { Value = false };
};

template <typename T> struct TIsTSubclassOf<               TSubclassOf<T>> { enum { Value = true }; };
template <typename T> struct TIsTSubclassOf<const          TSubclassOf<T>> { enum { Value = true }; };
template <typename T> struct TIsTSubclassOf<      volatile TSubclassOf<T>> { enum { Value = true }; };
template <typename T> struct TIsTSubclassOf<const volatile TSubclassOf<T>> { enum { Value = true }; };

/**
 * Template to allow UClass types to be passed around with type safety
 */
template <typename T>
class TSubclassOf
{
private:
	template <typename U>
	friend class TSubclassOf;

public:
	using ElementType = T;

	TSubclassOf() = default;
	TSubclassOf(TSubclassOf&&) = default;
	TSubclassOf(const TSubclassOf&) = default;
	TSubclassOf& operator=(TSubclassOf&&) = default;
	TSubclassOf& operator=(const TSubclassOf&) = default;
	~TSubclassOf() = default;

	/** Constructor that takes a UClass*. */
	FORCEINLINE TSubclassOf(UClass* From)
		: Class(From)
	{
	}

	/** Construct from a UClass* (or something implicitly convertible to it) */
	template <
		typename U,
		std::enable_if_t<
		!TIsTSubclassOf<std::decay_t<U>>::Value,
		decltype(ImplicitConv<UClass*>(std::declval<U>()))
		>* = nullptr
	>
	FORCEINLINE TSubclassOf(U&& From)
		: Class(From)
	{
	}

	/** Construct from another TSubclassOf, only if types are compatible */
	template <
		typename OtherT,
		decltype(ImplicitConv<T*>((OtherT*)nullptr))* = nullptr
	>
	FORCEINLINE TSubclassOf(const TSubclassOf<OtherT>& Other)
		: Class(Other.Class)
	{
		//IWYU_MARKUP_IMPLICIT_CAST(OtherT, T);
	}

	/** Assign from another TSubclassOf, only if types are compatible */
	template <
		typename OtherT,
		decltype(ImplicitConv<T*>((OtherT*)nullptr))* = nullptr
	>
	FORCEINLINE TSubclassOf& operator=(const TSubclassOf<OtherT>& Other)
	{
		//IWYU_MARKUP_IMPLICIT_CAST(OtherT, T);
		Class = Other.Class;
		return *this;
	}

	/** Assign from a UClass*. */
	FORCEINLINE TSubclassOf& operator=(UClass* From)
	{
		Class = From;
		return *this;
	}

	/** Assign from a UClass* (or something implicitly convertible to it). */
	template <
		typename U,
		std::enable_if_t<
		!TIsTSubclassOf<std::decay_t<U>>::Value,
		decltype(ImplicitConv<UClass*>(std::declval<U>()))
		>* = nullptr
	>
	FORCEINLINE TSubclassOf& operator=(U&& From)
	{
		Class = From;
		return *this;
	}

	/** Dereference back into a UClass*, does runtime type checking. */
	FORCEINLINE UClass* operator*() const
	{
		if (!Class || !Class->IsChildOf(T::StaticClass()))
		{
			return nullptr;
		}
		return Class;
	}

	/** Dereference back into a UClass*, does runtime type checking. */
	FORCEINLINE UClass* Get() const
	{
		return **this;
	}

	/** Dereference back into a UClass*, does runtime type checking. */
	FORCEINLINE UClass* operator->() const
	{
		return **this;
	}

	/** Implicit conversion to UClass*, does runtime type checking. */
	FORCEINLINE operator UClass* () const
	{
		return **this;
	}

	/**
	 * Get the CDO if we are referencing a valid class
	 *
	 * @return the CDO, or null if class is null
	 */
	FORCEINLINE T* GetDefaultObject() const
	{
		UObject* Result = nullptr;
		if (Class)
		{
			Result = Class->GetDefaultObject();
			//check(Result && Result->IsA(T::StaticClass()));
		}
		return (T*)Result;
	}

	//FORCEINLINE void Serialize(FArchive& Ar)
	//{
	//	Ar << Class;
	//}

	friend uint32 GetTypeHash(const TSubclassOf& SubclassOf)
	{
		return GetTypeHash(SubclassOf.Class);
	}

private:
	UClass* Class = nullptr;
};

//template <typename T>
//struct TCallTraits<TSubclassOf<T>> : public TCallTraitsBase<TSubclassOf<T>>
//{
//	using ConstPointerType = TSubclassOf<const T>;
//};

//template <typename T>
//FArchive& operator<<(FArchive& Ar, TSubclassOf<T>& SubclassOf)
//{
//	SubclassOf.Serialize(Ar);
//	return Ar;
//}

//#if UE_ENABLE_INCLUDE_ORDER_DEPRECATED_IN_5_2
//#include "CoreMinimal.h"
//#endif
