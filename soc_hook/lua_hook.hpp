#pragma once

#include "utility.hpp"
#include "lua/lua.h"
#include "lua/lauxlib.h"

namespace lua
{
	template <typename ReturnType>
	std::optional<ReturnType> get_address_from_ordinal(const int ordinal)
	{
		return ::get_address_from_ordinal<ReturnType>("xrlua.dll", ordinal);	
	}

	namespace original
	{
		extern decltype(lua_newthread)* newthread;
		extern decltype(lua_status)* status;
		extern decltype(luaL_loadfile)* loadfile;
		extern decltype(luaL_loadstring)* loadstring;
		extern decltype(luaL_loadbuffer)* loadbuffer;
		extern decltype(lua_pushstring)* pushstring;

		bool initialize();
	}

	namespace my
	{
	}
}