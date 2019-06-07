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
		decltype(lua_newthread)* newthread;
		decltype(luaopen_jit)* open_jit;

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

			if (const auto addr = get_address_from_ordinal<decltype(newthread)>(365))
			{
				newthread = *addr;
			}
			else
			{
				return false;
			}

			if (const auto addr = get_address_from_ordinal<decltype(open_jit)>(417))
			{
				open_jit = *addr;
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

		int open_jit(lua_State* state)
		{
			std::cout << "my::open_jit\n";

			const auto ret = original::open_jit(state);
			
			std::cout << "got past original::open_jit\n";
			const auto my_state = lua::original::newthread(state);

			std::cout << "got my_state = " << std::hex << my_state << '\n';

			if (my_state && !lua::original::loadfile(my_state, "some_test_lua.script"))
			{
				std::cout << "got past loadfile\n";
				lua::original::pcall(my_state, 0, -1, 0);
				std::cout << "got past pcall\n";
			}

			//original::loadfile(state, "some_test_lua.script") || original::pcall(state, 0, -1, 0);
			return ret;
		}
	}
}
