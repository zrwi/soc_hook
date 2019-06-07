#include "pch.h"
#include "utility.hpp"

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

std::optional<address> get_address_from_ordinal(LPCSTR module, const int ordinal)
{
	return get_address_from_ordinal<address>(module, ordinal);
}

std::optional<address> get_absolute_address_from_module(const HMODULE module, const std::string& name, const address offset)
{
	const auto a = address(module) + offset;
	std::cout << name << " is at " << std::hex << a << '\n';
	return a;
}

std::optional<address> get_absolute_address_from_core(const std::string& name, const address offset)
{
	const static auto xrcore_dll = get_module("xrCore.dll");

	if (!xrcore_dll)
	{
		std::cerr << "failed to get " << name << '\n';
		return std::nullopt;
	}

	return get_absolute_address_from_module(*xrcore_dll, name, offset);
}

std::optional<address> get_absolute_address_from_game(const std::string& name, const address offset)
{
	const static auto xrgame_dll = get_module("xrGame.dll");

	if (!xrgame_dll)
	{
		std::cerr << "failed to get " << name << '\n';
		return std::nullopt;
	}

	return get_absolute_address_from_module(*xrgame_dll, name, offset);
}