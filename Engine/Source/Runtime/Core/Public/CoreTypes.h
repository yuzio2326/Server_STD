#pragma once
#pragma warning(disable: 4251)
#pragma warning(disable: 4530) // exception warning
#include <string>
#include <format>
#include <memory>
#include <typeinfo>
#include <functional>
#include <codecvt>
#include <filesystem>
#include <chrono>
#include <span>
#include <cmath>
#include <atomic>
#include <thread>
#include <mutex>
#include <future>

#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

// Containers --------------
#include <vector>
#include <map>
#include <unordered_map>
#include <queue>
#include <set>
#include <stack>
// Containers End --------------

#include "UObject/UObjectMacros.h"
#include "HAL/Platform.h"
#include "UObject/NameTypes.h"

#include "Misc/CoreMiscDefines.h"

// Reflection
#include "Reflection/Reflection.h"

using namespace std;
using FString = std::wstring;
using FStringView = std::wstring_view;
template<class T>
using TArray = std::vector<T>;
template<class T, size_t _Size>
using TStaticArray = std::array<T, _Size>;
template<class T>
using TSet = std::set<T>;
template<class T>
using TObjectPtr = std::shared_ptr<T>;

static inline const WIDECHAR* NAME_NONE = TEXT("");
