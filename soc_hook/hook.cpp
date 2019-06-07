#include "pch.h"
#include "utility.hpp"

address original_console_log = 0;
void my_console_log(const char* c)
{
	//std::cout << "called my_console_log with \"" << c << "\"\n";

	const auto original = decltype(&my_console_log)(original_console_log);
	original(c);
}

/*
 * Address=050CEC24
Type=Import
Symbol=?rtc_decompress@@YAIPAXIPBXI@Z
Symbol (undecorated)=unsigned int __cdecl rtc_decompress(void *,unsigned int,void const *,unsigned int)
 */

void dump_decompressed_save_file(void* const buffer, const std::size_t length)
{
	std::ostringstream file_name;
	//G:\S.T.A.L.K.E.R. - Shadow of Chernobyl\bin\17250030_403f31.decompressed_save_file
	file_name << std::hex << buffer << '_' << length << ".decompressed_save_file";

	std::ofstream file{file_name.str(), std::ios_base::out | std::ios_base::binary};
	file.write((char*)buffer, length);
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

template <typename DataType>
struct vec3
{
	DataType x;
	DataType y;
	DataType z;
};

class game_string
{
public:
	char pad_0000[4]; //0x0000
	uint32_t length; //0x0004
	char pad_0008[4]; //0x0008
	char raw_string[1]; //0x000C
}; //Size: 0x001C

class game_object
{
public:
	char pad_0000[54]; //0x0000
	uint16_t id; //0x0036
	char pad_0038[8]; //0x0038
	game_string *name; //0x0040
	char pad_0044[20]; //0x0044
	vec3<float> position; //0x0058
	vec3<float> rotation; //0x0064
	char pad_0070[20]; //0x0070
}; //Size: 0x0084


address original_load_object = 0;
void __cdecl my_load_object(const address unknown_class_instance, game_object* const object)
{
	std::cout << std::hex << object << std::endl;

	//std::cout << object->id << " " << object->name->raw_string << '\n';

	const auto original = decltype(&my_load_object)(original_load_object);
	original(unknown_class_instance, object);
}

bool init_addresses()
{
	if (const auto address = get_address_from_ordinal("xrCore.dll", 173)) //  01029890 Export 173 ?Log@@YAXPBD@Z void __cdecl Log(char const *)
	{
		original_console_log = *address;
		std::cout << "found Log at " << std::hex << original_console_log << std::endl;
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

	if (const auto addr = get_absolute_address_from_game("load_object", 0x60b9c))
	{
		original_load_object = *addr;
	}
	else
	{
		return false;
	}

	return true;
}

void idle()
{
	std::cout << "Hold END to detach this DLL from the game.\n";
	while (!GetAsyncKeyState(VK_END))
	{
		Sleep(1024);
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
		{&(PVOID&)original_rtc_decompress, &my_rtc_decompress},
		{&(PVOID&)original_load_object, &my_load_object},
	};
 
	my_console_log("- Hook to game console is working!");

	idle(); // blocking
}