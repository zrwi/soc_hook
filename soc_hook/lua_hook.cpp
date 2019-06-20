#include "pch.h"
#include "utility.hpp"
#include "lua_hook.hpp"

namespace lua
{
	namespace original
	{
		decltype(luabind::detail::pcall)* pcall;

		template <typename ReturnType>
		std::optional<ReturnType> get_address_from_ordinal(const int ordinal)
		{
			return ::get_address_from_ordinal<ReturnType>("xrlua.dll", ordinal);	
		}

		bool initialize()
		{
			if (const auto address = get_address_from_ordinal<decltype(pcall)>(243))
			{
				pcall = *address;
			}
			else
			{
				return false;
			}

			return true;
		}
	}

	namespace my
	{

	}
}
