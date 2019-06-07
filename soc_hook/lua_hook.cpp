#include "pch.h"
#include "utility.hpp"
#include "lua_hook.hpp"

namespace lua
{
	namespace original
	{
		decltype(lua_gettop)* gettop;
		decltype(lua_pcall)* pcall;
		decltype(luaL_loadfile)* loadfile;

		template <typename ReturnType>
		std::optional<ReturnType> get_address_from_ordinal(const int ordinal)
		{
			return ::get_address_from_ordinal<ReturnType>("xrlua.dll", ordinal);	
		}

		bool initialize()
		{
			if (const auto addr = get_address_from_ordinal<decltype(gettop)>(354))
			{
				gettop = *addr;
			}
			else
			{
				return false;
			}

			if (const auto addr = get_address_from_ordinal<decltype(pcall)>(369))
			{
				pcall = *addr;
			}
			else
			{
				return false;
			}

			if (const auto addr = get_address_from_ordinal<decltype(loadfile)>(313))
			{
				loadfile = *addr;
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
		std::optional<lua_State*> g_state;

		int32_t gettop(lua_State* state)
		{
			assert(state != nullptr);

			if (!g_state)
			{
				g_state = state;
				std::cout << "g_state = " << std::hex << state << '\n';
			}

			return original::gettop(state);
		}
	}
}
