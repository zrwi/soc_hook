#pragma once

#include "pch.h"
#include "lua/lua.h"
#include "lua/lauxlib.h"
#include "lua/lualib.h"

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
		extern decltype(lua_gettop)* gettop;
		extern decltype(lua_pcall)* pcall;
		extern decltype(luaL_loadfile)* loadfile;
		extern decltype(lua_newthread)* newthread;
		extern decltype(luaopen_jit)* open_jit;

		bool initialize();
	}

	namespace my
	{
		extern std::optional<lua_State*> g_state;

		int gettop(lua_State* state);
		int open_jit(lua_State* state);
	}
}