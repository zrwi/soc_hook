#pragma once

#include "pch.h"

void print_last_error(std::ostream& output);
void print_last_error();

std::optional<HMODULE> get_module(LPCSTR module);

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

std::optional<address> get_address_from_ordinal(LPCSTR module, const int ordinal);

std::optional<address> get_absolute_address_from_module(const HMODULE module, const std::string& name, const address offset);
std::optional<address> get_absolute_address_from_module(const std::string& module, const std::string& name, const address offset);
std::optional<address> get_absolute_address_from_core(const std::string& name, const address offset);
std::optional<address> get_absolute_address_from_game(const std::string& name, const address offset);