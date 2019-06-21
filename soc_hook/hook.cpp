#include "pch.h"
#include "utility.hpp"

#include "lua_hook.hpp"
#include "luabind_hook.hpp"

address original_console_log = 0;
void my_console_log(const char* c)
{
	//std::cout << "called my_console_log with \"" << c << "\"\n";

	const auto original = decltype(&my_console_log)(original_console_log);
	original(c);
}

void dump_decompressed_save_file(void* const buffer, const std::size_t length)
{
	std::ostringstream file_name_oss;
	//G:\S.T.A.L.K.E.R. - Shadow of Chernobyl\bin\17250030_403f31.decompressed_save_file
	file_name_oss << std::hex << buffer << '_' << length << ".decompressed_save_file";

	const auto file_name = file_name_oss.str();
	
	std::ofstream file{file_name, std::ios_base::out | std::ios_base::binary};
	
	file.write((char*)buffer, length);
	
	std::cout << "saved " << file_name << '\n';
}

address original_rtc_decompress = 0;
unsigned int my_rtc_decompress(void* pointer_to_buffer_to_hold_decompressed_save_file, unsigned int decompressed_length,
							   void* pointer_to_save_file_data, unsigned int save_file_length)
{
	const auto original = decltype(&my_rtc_decompress)(original_rtc_decompress);
	const auto ret = original(pointer_to_buffer_to_hold_decompressed_save_file, decompressed_length, pointer_to_save_file_data, save_file_length);
	
	std::cout << "ptr_save_data = " << std::hex << pointer_to_save_file_data
			  << "\nsave_file_length = " << save_file_length
			  << "\nptr_decompressed_buffer = " << pointer_to_buffer_to_hold_decompressed_save_file
			  << "\ndecompressed_length = " << decompressed_length
			  << "\n\n";

	dump_decompressed_save_file(pointer_to_buffer_to_hold_decompressed_save_file, decompressed_length);

	return ret;
}

bool init_addresses()
{
	if (const auto addr = get_address_from_ordinal<address>("xrcore.dll", 173))
	{
		original_console_log = *addr;
	}
	else
	{
		return false;
	}

	if (const auto addr = get_absolute_address_from_core("rtc_decompress", 0x126f0))
	{
		original_rtc_decompress = *addr;
	}
	else
	{
		return false;
	}

	if (!lua::original::initialize())
	{
		return false;
	}

	if (!luabind::original::initialize())
	{
		return false;
	}

	return true;
}


void hack()
{
	using namespace luabind;
	using namespace luabind::original;
	using namespace lua::original;

	if (!my::state)
	{
		my::initialize_new_state();
		return;
	}

	const auto state = *my::state;

	if (const auto s = status(state))
	{
		std::cout << "hack(): lua::original::status(*my::state) == " << s << '\n';
		return;
	}

	pushstring(state, "hack_numpad_press(\"NUM0\");");
	pcall(state, 0, 0);

	/*
		char buffer[48];
		memset(buffer, NULL, 48);

		sprintf_s(buffer, "hack_numpad_press(\"%s\");", key_str[i - 0x60]);

		o_lua_pushstring(hack_lua, buffer);
		o_luabind_pcall(hack_lua, 0, 0);
	*/
}

void idle()
{
	std::cout << "Hold END to detach this DLL from the game.\n";

	while (!GetAsyncKeyState(VK_END))
	{
		if ( (GetAsyncKeyState(VK_INSERT) & 1))
		{
			hack();
		}

		Sleep(256);
	}
}

void wait_for_user_input()
{
	char sentinel;
	std::cin >> sentinel;
}

void hook_and_idle()
{
	if (!init_addresses())
	{
		wait_for_user_input();
		return;
	}

	invasive_hooks h
	{
		//{&(PVOID&)original_function_address, &my_function_to_call},
		{&(PVOID&)original_console_log, &my_console_log},
		//{&(PVOID&)original_rtc_decompress, &my_rtc_decompress},
		{&(PVOID&)luabind::original::pcall, &luabind::my::pcall},
	};
 
	my_console_log("- Hook to game console is working!");

	idle(); // blocking
}