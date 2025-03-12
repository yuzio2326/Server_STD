#pragma once
#include "CoreTypes.h"

// weak_ptr의 사용성을 개선한 EnginePtr 입니다.
template<class _Ty>
class TEnginePtr : public weak_ptr<_Ty>
{
public:
	TEnginePtr() noexcept {}
	TEnginePtr(nullptr_t) noexcept {}
	TEnginePtr(const TEnginePtr& InOther) noexcept : weak_ptr<_Ty>(InOther) {}
	template <class _Ty2, enable_if_t<_SP_pointer_compatible<_Ty2, _Ty>::value, int> = 0>
	TEnginePtr(const weak_ptr<_Ty2>& InOther) noexcept : weak_ptr<_Ty>(InOther) {}

	template <class _Ty2, enable_if_t<_SP_pointer_compatible<_Ty2, _Ty>::value, int> = 0>
	TEnginePtr(const TObjectPtr<_Ty2>& InOther) noexcept : weak_ptr<_Ty>(InOther) {}

	TEnginePtr& operator=(nullptr_t) noexcept {
		this->reset();
		return *this;
	}

	TEnginePtr& operator=(const TEnginePtr& InOther) noexcept
	{
		TEnginePtr(InOther).swap(*this);
		return *this;
	}

	template <class _Ty2, enable_if_t<_SP_pointer_compatible<_Ty2, _Ty>::value, int> = 0>
	TEnginePtr& operator=(const TEnginePtr<_Ty2>& _Right) noexcept {
		TEnginePtr(_Right).swap(*this);
		return *this;
	}

	TEnginePtr& operator=(TEnginePtr&& _Right) noexcept {
		TEnginePtr(_STD move(_Right)).swap(*this);
		return *this;
	}

	template <class _Ty2, enable_if_t<_SP_pointer_compatible<_Ty2, _Ty>::value, int> = 0>
	TEnginePtr& operator=(TEnginePtr<_Ty2>&& _Right) noexcept {
		TEnginePtr(_STD move(_Right)).swap(*this);
		return *this;
	}

	template <class _Ty2, enable_if_t<_SP_pointer_compatible<_Ty2, _Ty>::value, int> = 0>
	TEnginePtr& operator=(const TObjectPtr<_Ty2>& _Right) noexcept {
		TEnginePtr(_Right).swap(*this);
		return *this;
	}

	template <class _Ty2 = _Ty, enable_if_t<!disjunction_v<is_array<_Ty2>, is_void<_Ty2>>, int> = 0>
	_NODISCARD _Ty2& operator*() const noexcept {
		return *Get();
	}

	template <class _Ty2 = _Ty, enable_if_t<!is_array_v<_Ty2>, int> = 0>
	_NODISCARD _Ty2* operator->() const noexcept {
		return Get();
	}
	template <class _Ty2 = _Ty, enable_if_t<!is_array_v<_Ty2>, int> = 0>
	_NODISCARD _Ty2* Get() const noexcept {
		return this->lock().get();
	}
	template <class _Ty2 = _Ty, enable_if_t<!is_array_v<_Ty2>, int> = 0>
	_NODISCARD _Ty2* GetChecked() const noexcept {
#if WITH_DEBUG
		if (!IsValid())
		{
			_ASSERT(false);
		}
#endif
		return this->lock().get();
	}
	FORCEINLINE operator _Ty* () const { return Get(); }
	FORCEINLINE _Ty* operator->() const { return Get(); }
	FORCEINLINE _Ty& operator*() const { return *Get(); }

	explicit operator bool() const noexcept
	{
		return IsValid();
	}

	bool IsValid() const noexcept
	{
		if (!this) { return false; }
		return !this->expired();
	}
};

template<class _Ty1, class _Ty2>
bool operator==(const TEnginePtr<_Ty1> _Left, const TEnginePtr<_Ty2> _Right)
{
	return _Left.Get() == _Right.Get();
}
template<class _Ty1, class _Ty2>
bool operator!=(const TEnginePtr<_Ty1> _Left, const TEnginePtr<_Ty2> _Right)
{
	return _Left.Get() != _Right.Get();
}
template<class _Ty1>
bool operator==(const TEnginePtr<_Ty1> _Left, nullptr_t)
{
	return _Left.Get() == nullptr;
}
template<class _Ty1>
bool operator!=(const TEnginePtr<_Ty1> _Left, nullptr_t)
{
	return _Left.Get() != nullptr;
}