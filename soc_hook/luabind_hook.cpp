#include "pch.h"
#include "luabind_hook.hpp"
#include "lua_hook.hpp"

namespace luabind
{
	namespace original
	{
		std::optional<lua_State*> state;

		decltype(luabind::detail::pcall)* pcall;

		bool initialize()
		{
			if (const auto address = lua::get_address_from_ordinal<decltype(pcall)>(243))
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
		std::optional<lua_State*> state;

		int pcall(lua_State *L, int nargs, int nresults)
		{
			if (!original::state)
			{
				original::state = L;
				std::cout << "luabind::original::state == " << std::hex << *original::state << '\n';
			}

			return original::pcall(L, nargs, nresults);
		}

		std::optional<std::string> read_to_string(const std::string& filename)
		{
			std::ifstream file {filename};

			if (!file)
			{
				return std::nullopt;
			}

			using file_iterator = std::istreambuf_iterator<char>;

			return std::string
			{
				file_iterator(file),
				file_iterator()
			};
		}

		void load_script(const std::string& script_filename)
		{
			if (!state)
			{
				std::cout << "load_script(): my::state is not initialized.\n";
				return;
			}

			const auto script = read_to_string(script_filename);

			if (!script)
			{
				std::cout << "load_script(): couldn't find or open file \"" << script_filename << "\"\n";
				return;
			}

			lua::original::loadbuffer(*state, script->c_str(), script->size(), "hack_chunk");
			original::pcall(*state, 0, 0);

			std::cout << "load_script(): loaded \"" << script_filename << "\"\n";
		}

		void initialize_new_state()
		{
			if (state)
			{
				std::cout << "I've already a state.\n";
				return;
			}

			if (!original::state)
			{
				std::cout << "I haven't the original lua state.\n";
				return;
			}

			state = lua::original::newthread(*original::state);

			if (!state)
			{
				std::cout << "Got a null pointer out of newthread.\n";
				return;
			}

			std::cout << "luabind::my::state == " << std::hex << *state << '\n';

			load_script("test_lua_script.lua");
		}
	}
}