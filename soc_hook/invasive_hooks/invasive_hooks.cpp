#include "pch.h"

invasive_hooks::invasive_hooks(std::initializer_list<detour> detours) : detours_(detours)
{
	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());

	for (const auto& detour : detours)
	{
		DetourAttach(detour.first, detour.second);
	}

	DetourTransactionCommit();
}

invasive_hooks::~invasive_hooks()
{
	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());

	for (const auto& detour : detours_)
	{
		DetourDetach(detour.first, detour.second);
	}

	DetourTransactionCommit();
}
