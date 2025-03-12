#include "UObject/UObjectMacros.h"
#include "Windows/WindowsPlatform.h"
#include <typeindex>
#include <unordered_map>

namespace meta
{
	namespace internal
	{
		struct type_node;

		CORE_API std::unordered_map<std::type_index, meta::internal::type_node*>& GetReflectionMap()
		{
			static std::unordered_map<std::type_index, meta::internal::type_node*> MapReflection;
			return MapReflection;
		}
		CORE_API meta::internal::type_node** GetLastNode()
		{
			static meta::internal::type_node* Node;
			return &Node;
		}
	}
}