#pragma once

#include "luabind/detail/pcall.hpp"

namespace luabind
{
	namespace original
	{
		extern std::optional<lua_State*> state;
		
		extern decltype(luabind::detail::pcall)* pcall;

		bool initialize();
	}

	namespace my
	{
		extern std::optional<lua_State*> state;

		int pcall(lua_State *L, int nargs, int nresults);

		void initialize_new_state();
	}
}