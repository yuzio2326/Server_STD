#pragma once
#include "Public/GenericPlatform/GenericPlatform.h"

/**
* Windows specific types
**/
struct FWindowsPlatformTypes : public FGenericPlatformTypes
{
#ifdef _WIN64
	typedef unsigned __int64	SIZE_T;
	typedef __int64				SSIZE_T;
#else
	typedef unsigned long		SIZE_T;
	typedef long				SSIZE_T;
#endif

#if USE_UTF8_TCHARS
	typedef UTF8CHAR TCHAR;
#endif
};

typedef FWindowsPlatformTypes FPlatformTypes;

// Q: Why is there a __nop() before __debugbreak()?
// A: VS' debug engine has a bug where it will silently swallow explicit
// breakpoint interrupts when single-step debugging either line-by-line or
// over call instructions. This can hide legitimate reasons to trap. Asserts
// for example, which can appear as if the did not fire, leaving a programmer
// unknowingly debugging an undefined process.
#define PLATFORM_BREAK() (__nop(), __debugbreak())

#define E_DEBUG_BREAK_IMPL() PLATFORM_BREAK()

#if UE_BUILD_SHIPPING && !WITH_EDITOR
#define E_DEBUG_BREAK() ((void)0)
#else
#define E_DEBUG_BREAK() ((void)(IsDebuggerPresent() && ([] () { E_DEBUG_BREAK_IMPL(); } (), 1)))
#endif

#define MS_ALIGN(n) __declspec(align(n))