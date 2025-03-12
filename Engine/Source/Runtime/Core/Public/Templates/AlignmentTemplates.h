#pragma once
#include "CoreTypes.h"


/**
 * Traits class which tests if a type is integral.
 */
template <typename T>
struct TIsIntegral
{
    enum { Value = false };
};

template <> struct TIsIntegral<         bool> { enum { Value = true }; };
template <> struct TIsIntegral<         char> { enum { Value = true }; };
template <> struct TIsIntegral<signed   char> { enum { Value = true }; };
template <> struct TIsIntegral<unsigned char> { enum { Value = true }; };
template <> struct TIsIntegral<         char16_t> { enum { Value = true }; };
template <> struct TIsIntegral<         char32_t> { enum { Value = true }; };
template <> struct TIsIntegral<         wchar_t> { enum { Value = true }; };
template <> struct TIsIntegral<         short> { enum { Value = true }; };
template <> struct TIsIntegral<unsigned short> { enum { Value = true }; };
template <> struct TIsIntegral<         int> { enum { Value = true }; };
template <> struct TIsIntegral<unsigned int> { enum { Value = true }; };
template <> struct TIsIntegral<         long> { enum { Value = true }; };
template <> struct TIsIntegral<unsigned long> { enum { Value = true }; };
template <> struct TIsIntegral<         long long> { enum { Value = true }; };
template <> struct TIsIntegral<unsigned long long> { enum { Value = true }; };

template <typename T> struct TIsIntegral<const          T> { enum { Value = TIsIntegral<T>::Value }; };
template <typename T> struct TIsIntegral<      volatile T> { enum { Value = TIsIntegral<T>::Value }; };
template <typename T> struct TIsIntegral<const volatile T> { enum { Value = TIsIntegral<T>::Value }; };


/**
 * Traits class which tests if a type is a pointer.
 */
template <typename T>
struct TIsPointer
{
    enum { Value = false };
};

template <typename T> struct TIsPointer<T*> { enum { Value = true }; };

template <typename T> struct TIsPointer<const          T> { enum { Value = TIsPointer<T>::Value }; };
template <typename T> struct TIsPointer<      volatile T> { enum { Value = TIsPointer<T>::Value }; };
template <typename T> struct TIsPointer<const volatile T> { enum { Value = TIsPointer<T>::Value }; };


/**
 * 값을 가장 가까운 상위의 'Alignment' 배수로 정렬합니다. 'Alignment'는 2의 제곱이어야 합니다.
 *
 * @param  Val        정렬할 값.
 * @param  Alignment  정렬 값, 2의 제곱이어야 합니다.
 *
 * @return 지정된 정렬에 맞춰 정렬된 값.
 */
template <typename T>
FORCEINLINE constexpr T Align(T Val, uint64 Alignment)
{
    static_assert(TIsIntegral<T>::Value || TIsPointer<T>::Value, "Align expects an integer or pointer type");

    return (T)(((uint64)Val + Alignment - 1) & ~(Alignment - 1));
}

#define STRUCT_OFFSET( struc, member )	offsetof(struc, member)