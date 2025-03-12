#pragma once
#include "UObject/Object.h"

template<class ... Args>
class FDelegate
{
public:
	template<class TType>
	void AddRow(TType* NewInstance, void(TType::* NewFunction)(Args ...))
	{
		FFunctionType Function;
		constexpr uint32 Argc = sizeof ...(Args);
		if constexpr (Argc == 0) { Function = bind(NewFunction, NewInstance); }
		else if constexpr (Argc == 1) { Function = bind(NewFunction, NewInstance, std::placeholders::_1); }
		else if constexpr (Argc == 2) { Function = bind(NewFunction, NewInstance, std::placeholders::_1, std::placeholders::_2); }
		else if constexpr (Argc == 3) { Function = bind(NewFunction, NewInstance, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3); }
		else if constexpr (Argc == 4) { Function = bind(NewFunction, NewInstance, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4); }
		else if constexpr (Argc == 5) { Function = bind(NewFunction, NewInstance, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5); }
		else if constexpr (Argc == 6) { Function = bind(NewFunction, NewInstance, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5, std::placeholders::_6); }
		else if constexpr (Argc == 7) { Function = bind(NewFunction, NewInstance, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5, std::placeholders::_6, std::placeholders::_7); }
		else if constexpr (Argc == 8) { Function = bind(NewFunction, NewInstance, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5, std::placeholders::_6, std::placeholders::_7, std::placeholders::_8); }
		else if constexpr (Argc == 9) { Function = bind(NewFunction, NewInstance, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5, std::placeholders::_6, std::placeholders::_7, std::placeholders::_8, std::placeholders::_9); }
		else if constexpr (Argc == 10) { Function = bind(NewFunction, NewInstance, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5, std::placeholders::_6, std::placeholders::_7, std::placeholders::_8, std::placeholders::_9, std::placeholders::_10); }

		Functions.emplace_back(NewInstance, Function);
	}

	void Broadcast(Args... InArgs)
	{
		for (auto It : Functions)
		{
			It.second(forward<Args>(InArgs)...);
		}
	}

	void Clear()
	{
		Functions.clear();
	}

private:
	using FFunctionType = function<void(Args ...)>;
	TArray<pair<void*, FFunctionType>> Functions;
};


template<typename ... Args>
class FSafeDelegate
{
public:
	using FFunctionType = std::function<void(Args ...)>;

	template<typename TObject>
	bool IsDuplicateObject(TObject* NewInstance)
	{
		for (const auto& Pair : Functions)
		{
			UObject* ExistingInstance = Pair.first.Get();
			if (ExistingInstance == NewInstance)
			{
				_ASSERT(false);
				//E_LOG(Error, TEXT("The same object instance is already added."));
				return true;
			}
		}
		return false;
	}
	template<typename TObject>
	void AddUObject(TObject* NewInstance, void(TObject::* NewFunction)(Args ...))
	{
		static_assert(std::is_base_of<UObject, TObject>::value, "TObject must inherit from UObject");
		if (IsDuplicateObject(NewInstance)) { return; }

		FFunctionType F;
		constexpr uint32 Argc = sizeof ... (Args);
		if constexpr (Argc == 0) { F = bind(NewFunction, NewInstance); }
		else if constexpr (Argc == 1) { F = bind(NewFunction, NewInstance, std::placeholders::_1); }
		else if constexpr (Argc == 2) { F = bind(NewFunction, NewInstance, std::placeholders::_1, std::placeholders::_2); }
		else if constexpr (Argc == 3) { F = bind(NewFunction, NewInstance, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3); }
		else if constexpr (Argc == 4) { F = bind(NewFunction, NewInstance, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4); }
		else if constexpr (Argc == 5) { F = bind(NewFunction, NewInstance, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5); }
		else if constexpr (Argc == 6) { F = bind(NewFunction, NewInstance, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5, std::placeholders::_6); }
		else if constexpr (Argc == 7) { F = bind(NewFunction, NewInstance, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5, std::placeholders::_6, std::placeholders::_7); }
		else if constexpr (Argc == 8) { F = bind(NewFunction, NewInstance, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5, std::placeholders::_6, std::placeholders::_7, std::placeholders::_8); }
		else if constexpr (Argc == 9) { F = bind(NewFunction, NewInstance, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5, std::placeholders::_6, std::placeholders::_7, std::placeholders::_8, std::placeholders::_9); }
		else if constexpr (Argc == 10) { F = bind(NewFunction, NewInstance, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5, std::placeholders::_6, std::placeholders::_7, std::placeholders::_8, std::placeholders::_9, std::placeholders::_10); }

		TEnginePtr<UObject> WeakObjectPtr = NewInstance->As<UObject>();
		Functions.push_back({ WeakObjectPtr, F });
	}

	void Broadcast(Args... InArgs)
	{
		auto It = Functions.begin();
		while (It != Functions.end())
		{
			if (auto WeakInstanceWithValidCheck = It->first)
			{
				It->second(std::forward<Args>(InArgs)...);
				++It;
			}
			else
			{
				It = Functions.erase(It);
			}
		}
	}

private:
	TArray<std::pair<TEnginePtr<UObject>, FFunctionType>> Functions;
};