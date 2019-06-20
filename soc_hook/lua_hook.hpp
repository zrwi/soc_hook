#pragma once

#include "pch.h"
#include "lua/lua.h"
#include "lua/lauxlib.h"
#include "lua/lualib.h"
#include "luabind/detail/pcall.hpp"


namespace lua
{
	namespace original
	{
		extern decltype(luabind::detail::pcall)* pcall;

		bool initialize();
	}

	namespace my
	{
	}
}