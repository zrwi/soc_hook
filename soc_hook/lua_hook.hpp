#pragma once

#include "pch.h"
#include "lua-5.1.1/src/lua.h"
#include "lua-5.1.1/src/lauxlib.h"

namespace lua
{
	//struct state
	//{
	//	static std::optional<state*> instance;

	//	int8_t pad12[12];
	//	int32_t f12;
	//	int32_t f16;
	//};

	namespace original
	{
		bool initialize();

		extern decltype(lua_gettop)* gettop;
		extern decltype(lua_pcall)* pcall;
		extern decltype(luaL_loadfile)* loadfile;
	}

	namespace my
	{
		int gettop(lua_State* state);
	}
}