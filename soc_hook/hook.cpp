#include "pch.h"

using address = uintptr_t;

void print_last_error(std::ostream& output)
{
	// todo: string error message from error code
	output << "Error code is " << GetLastError() << std::endl;
}

void print_last_error()
{
	print_last_error(std::cerr);
}

std::optional<HMODULE> get_module(LPCSTR module)
{
	const auto module_address = GetModuleHandleA(module);
	
	if (module_address)
	{
		return module_address;
	}

	std::cerr << "Failed to get module address of " << module << ".\n";
	print_last_error();
	return std::nullopt;
}

template <typename ReturnType>
std::optional<ReturnType> get_address_from_ordinal(LPCSTR module, const int ordinal)
{
	const auto module_address = get_module(module);

	if (!module_address)
	{
		return std::nullopt;
	}

	const auto ordinal_int_resource = MAKEINTRESOURCEA(ordinal);
	const auto address = GetProcAddress(*module_address, ordinal_int_resource);

	if (!address)
	{
		std::cerr << "Failed to get address of function with ordinal " << ordinal << " in module " << module << ".\n";
		print_last_error();
		return std::nullopt;	
	}

	return ReturnType(address);
}

std::optional<address> get_address_from_ordinal(LPCSTR module, const int ordinal)
{
	return get_address_from_ordinal<address>(module, ordinal);
}

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

auto get_from_core(const std::string& name, const int ordinal)
{
	const auto address = get_address_from_ordinal("xrCore.dll", ordinal);

	if (address)
	{
		std::cout << "found " << name << " at " << std::hex << *address << std::endl;
	}

	return address;
}

bool init_addresses()
{
	if (const auto address = get_from_core("console_log", 173)) //  01029890 Export 173 ?Log@@YAXPBD@Z void __cdecl Log(char const *)
	{
		original_console_log = *address;
	}
	else
	{
		return false;
	}

	if (const auto xrcore_dll = get_module("xrCore.dll"))
	{
		original_rtc_decompress = address(*xrcore_dll) + 0x126f0;
		std::cout << "found rtc_decompress at " << std::hex << original_rtc_decompress << '\n';
	}
	else
	{
		std::cerr << "unable to find rtc_decomrpess\n";
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

void hook_and_idle()
{
	if (!init_addresses())
	{
		return;
	}

	invasive_hooks h
	{
		//{&(PVOID&)original_function_address, &my_function_to_call},
		{&(PVOID&)original_console_log, &my_console_log},
		{&(PVOID&)original_rtc_decompress, &my_rtc_decompress},
	};
 
	my_console_log("- Hook to game console is working!");

	idle(); // blocking
}