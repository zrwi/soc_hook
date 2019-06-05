#include "pch.h"

using address = uintptr_t;

const int console_log_ORD = 173;

void print_last_error(std::ostream& output)
{
	// todo: string error message from error code
	output << "Error code is " << GetLastError() << std::endl;
}

void print_last_error()
{
	print_last_error(std::cerr);
}

template <typename ReturnType>
std::optional<ReturnType> get_address_from_ordinal(LPCSTR module, const int ordinal)
{
	const auto module_address = GetModuleHandleA(module);
	
	if (!module_address)
	{
		std::cerr << "Failed to get module address of " << module << ".\n";
		print_last_error();
		return std::nullopt;
	}

	const auto ordinal_int_resource = MAKEINTRESOURCEA(ordinal);
	const auto address = GetProcAddress(module_address, ordinal_int_resource);

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
	std::cout << "called my_console_log with \"" << c << "\"\n";

	const auto original = decltype(&my_console_log)(original_console_log);
	original(c);
}

struct 
{
	address p_call;
	address load_string;
	address to_lstring;
	address open_debug;
} original_lua;

bool init_addresses()
{
	const auto get_from_core = [](const int ordinal)
	{
		return get_address_from_ordinal("xrCore.dll", ordinal);
	};

	//const auto get_from_lua = [](const int ordinal)
	//{
	//	return get_address_from_ordinal("xrLUA.dll", ordinal);
	//};

	if (const auto address = get_from_core(console_log_ORD))
	{
		original_console_log = *address;
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
	};

	idle(); // blocking
}