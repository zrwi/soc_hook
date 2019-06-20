#pragma once

#include "pch.h"
#include "lua/lua.h"
#include "lua/lauxlib.h"
#include "lua/lualib.h"

namespace lua
{
	namespace original
	{
		extern decltype(lua_gettop)* gettop;
		extern decltype(lua_pcall)* pcall;
		extern decltype(luaL_loadfile)* loadfile;
		extern decltype(lua_newthread)* newthread;
		extern decltype(luaopen_jit)* open_jit;

		bool initialize();
	}

	namespace my
	{
	}
}