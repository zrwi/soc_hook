// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
#include "hook.hpp"

void create_console()
{
	AllocConsole();
	freopen_s((FILE**)stdout, "CONOUT$", "w", stdout);
	freopen_s((FILE**)stderr, "CONOUT$", "w", stderr);

	SetConsoleTitleA("debug");
	std::ios_base::sync_with_stdio(false);
}

void free_console()
{
	fclose(stdout);
	fclose(stderr);
	FreeConsole();
}

// ReSharper disable once CppParameterMayBeConst
DWORD WINAPI my_thread(LPVOID dll)
{
	create_console();

	std::cout << "My console is working!\n";

	hook_and_idle();
	
	free_console();
	FreeLibraryAndExitThread(HMODULE(dll), 0);
}

// ReSharper disable once CppInconsistentNaming
BOOL APIENTRY DllMain(const HMODULE dll, const DWORD reason, LPVOID /*unused*/)
{
	if (reason == DLL_PROCESS_ATTACH)
	{
		DisableThreadLibraryCalls(dll);
		CreateThread(nullptr, 0, my_thread, dll, 0, nullptr);
	}

	return TRUE;
}