#include "pch.h"
#include "lua_hook.hpp"

namespace lua
{
	namespace original
	{
		decltype(lua_newthread)* newthread;
		decltype(lua_status)* status;
		decltype(luaL_loadfile)* loadfile;
		decltype(luaL_loadstring)* loadstring;
		decltype(luaL_loadbuffer)* loadbuffer;
		decltype(lua_pushstring)* pushstring;

		bool initialize()
		{
			if (const auto address = get_address_from_ordinal<decltype(newthread)>(365))
			{
				newthread = *address;
			}
			else
			{
				return false;
			}

			if (const auto address = get_address_from_ordinal<decltype(status)>(399))
			{
				status = *address;
			}
			else
			{
				return false;
			}

			if (const auto address = get_address_from_ordinal<decltype(loadfile)>(313))
			{
				loadfile = *address;
			}
			else
			{
				return false;
			}

			if (const auto address = get_address_from_ordinal<decltype(loadstring)>(314))
			{
				loadstring = *address;
			}
			else
			{
				return false;
			}

			if (const auto address = get_address_from_ordinal<decltype(loadbuffer)>(312))
			{
				loadbuffer = *address;
			}
			else
			{
				return false;
			}

			if (const auto address = get_address_from_ordinal<decltype(pushstring)>(379))
			{
				pushstring = *address;
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
