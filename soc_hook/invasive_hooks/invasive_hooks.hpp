#pragma once

#include "pch.h"

class invasive_hooks
{
	using original = PVOID * ;
	using hook = PVOID;
	using detour = std::pair<original, hook>;

public:
	invasive_hooks(std::initializer_list<detour> detours);

	~invasive_hooks();

	invasive_hooks(const invasive_hooks&) = delete;
	invasive_hooks& operator=(const invasive_hooks&) = delete;

	invasive_hooks(invasive_hooks&&) = default;
	invasive_hooks& operator=(invasive_hooks&&) = default;

private:
	std::initializer_list<detour> detours_;
};