// dllmain.cpp: Definuje vstupní bod pro aplikaci knihovny DLL.
#include "stdafx.h"
#include <cstdio>
#include <cstdlib>
#include "detours.h"
#include <malloc.h>

/* thread status */
#define LUA_OK          0
#define LUA_YIELD       1
#define LUA_ERRRUN      2
#define LUA_ERRSYNTAX   3
#define LUA_ERRMEM      4
#define LUA_ERRGCMM     5
#define LUA_ERRERR      6

#pragma comment(lib, "detours.lib")

class Logging {
private:
	FILE * log;
public:
	Logging(const char* file) {
		fopen_s(&log, file, "w+");

		if (!log) {
			return;
		}
	}

	~Logging() {
		if(log) fclose(log);
		log = NULL;
	}

	void WriteToLog(const char* buf) {
		if (!buf) return;

		fputs(buf, log);
	}
};

class FileReader {
private:
	FILE* f;
public:
	FileReader(const char* file) {
		fopen_s(&f, file, "r");

		if (!f) {
			return;
		}
	}

	~FileReader() {
		if (f) fclose(f);
		f = NULL;
	}

	size_t GetLength() {
		fseek(f, 0, SEEK_END);
		size_t endlen = ftell(f);
		fseek(f, 0, SEEK_SET);
		return endlen;
	}

	void ReadToEnd(char* buf, size_t len) {
		if (!buf) return;
		if (!f) return;

		fread(buf, 1, len, f);
	}
};

Logging* mylog = NULL;
FileReader* myscript = NULL;
HANDLE ConsoleHandle = NULL;

char* script_buffer = NULL;

typedef struct lua_State lua_State;
typedef void *(__cdecl* lua_Alloc) (void *, void *, size_t, size_t);
typedef const char * (*lua_Reader) (lua_State *, void *, size_t *);
typedef int (__cdecl *p_luabind_pcall) (lua_State*, int, int);
typedef int (__cdecl* p_luaL_loadbuffer) (lua_State*, const char*, size_t, const char*);
typedef lua_State* (__cdecl* p_lua_newthread) (lua_State*);
typedef int (__cdecl* p_lua_resume) (lua_State*, int);
typedef int(__cdecl* p_lua_pushstring) (lua_State*, const char*);
typedef int(__cdecl* p_lua_status) (lua_State*);

lua_State* game_lua = NULL;
lua_State* hack_lua = NULL;

p_luabind_pcall o_luabind_pcall = NULL;
const unsigned int luabind_pcall_ordinal = 243; // int __cdecl luabind::detail::pcall(struct lua_State *, int, int) public ? pcall@detail@luabind@@YAHPAUlua_State@@HH@Z

p_luaL_loadbuffer o_luaL_loadbuffer = NULL;
const unsigned int luaL_loadbuffer_ordinal = 312; // int luaL_loadbuffer (lua_State *L, const char* buff, size_t sz, const char* name);

p_lua_newthread o_lua_newthread = NULL;
const unsigned int lua_newthread_ordinal = 365; // 

p_lua_resume o_lua_resume = NULL;
const unsigned int lua_resume_ordinal = 390; // 
													
p_lua_pushstring o_lua_pushstring = NULL;
const unsigned int lua_pushstring_ordinal = 314; // 

p_lua_status o_lua_status = NULL;
const unsigned int lua_status_ordinal = 399; // 

void Write(const char* c) {
	mylog->WriteToLog(c);
	WriteConsoleA(ConsoleHandle, c, strlen(c), NULL, NULL);
}

void* get_xrlua_export(unsigned int ord_number) {
	HMODULE xrlua_dll = GetModuleHandleA("xrLUA.dll");
	void* exp = NULL;

	if (xrlua_dll != NULL) {
		exp = GetProcAddress(xrlua_dll, MAKEINTRESOURCEA(ord_number));
	}

	return exp;
}

int __cdecl orig_luabind_pcall(lua_State *L, int nargs, int nrets) {
	if (game_lua == NULL) {
		game_lua = L;

		char buffer[256];
		memset(buffer, NULL, 256);

		sprintf_s(buffer, "xrLUA.dll -> lua_State = 0x%p\n", L);
		Write(buffer);
	}

	return o_luabind_pcall(L, nargs, nrets);
}

bool load_done = false;
bool key_pressed = false;
const char* key_str[] = { "NUM0", "NUM1", "NUM2", "NUM3", "NUM4", "NUM5", "NUM6", "NUM7", "NUM8", "NUM9", "" };

void HackLoop() {
	while (true) {
		if (!load_done) {
			Sleep(1000);
			hack_lua = o_lua_newthread(game_lua);

			if (!hack_lua) {
				Write("Unable to create new Lua thread\n");
				return;
			}
			else {
				Write("Hack Lua thread created\n");
			}

			o_luaL_loadbuffer(hack_lua, script_buffer, strlen(script_buffer), "hack_chunk");
			o_luabind_pcall(hack_lua, 0, 0);
			load_done = true;
		}

		int key = 0;

		for (int i = 0x60; i < 0x69; i++) {
			if (GetAsyncKeyState(i) & 1) {
				char buffer[48];
				memset(buffer, NULL, 48);

				sprintf_s(buffer, "hack_numpad_press(\"%s\");", key_str[i - 0x60]);

				int status = o_lua_status(hack_lua);

				if (status == LUA_OK) {
					o_lua_pushstring(hack_lua, buffer);
					o_luabind_pcall(hack_lua, 0, 0);
				}
				else {
					Write("Unable to call lua function in hack thread, ERR: ");
					_itoa_s(status, buffer, 10);
					Write(buffer);
					Write("\n");
				}

				break;
			}
		}

		Sleep(10);
	}
}

void ReadScriptFile() {
	size_t buflen = 0;

	buflen = myscript->GetLength();
	script_buffer = (char*)malloc(buflen + 1);

	memset(script_buffer, NULL, buflen + 1);

	myscript->ReadToEnd(script_buffer, buflen);
}

void HackInit() {
	mylog = new Logging("log_stalker_soc_hack.txt");
	myscript = new FileReader("test_file.txt");

	ReadScriptFile();

	ConsoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
	Write("Test hack by PhoenixDC\n");

	o_luaL_loadbuffer = (p_luaL_loadbuffer)get_xrlua_export(luaL_loadbuffer_ordinal);
	o_lua_newthread = (p_lua_newthread)get_xrlua_export(lua_newthread_ordinal);
	o_lua_resume = (p_lua_resume)get_xrlua_export(lua_resume_ordinal);
	o_lua_pushstring = (p_lua_pushstring)get_xrlua_export(lua_pushstring_ordinal);
	o_lua_status = (p_lua_status)get_xrlua_export(lua_status_ordinal);
	o_luabind_pcall = (p_luabind_pcall)DetourFunction((PBYTE)get_xrlua_export(luabind_pcall_ordinal), (PBYTE)orig_luabind_pcall);

	if (o_luabind_pcall != NULL) {
			Write("Hack loaded successfully\n");

			HANDLE thr_loop = CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)HackLoop, NULL, NULL, NULL);
	}
	else {
			Write("Failed to get lua_State\n");
	}
}

BOOL APIENTRY DllMain(HMODULE hModule,	DWORD  ul_reason_for_call, LPVOID lpReserved) {
    switch (ul_reason_for_call) {
		case DLL_PROCESS_ATTACH:
			AllocConsole(); 
			HackInit(); 
			break;

		case DLL_THREAD_ATTACH:
			break;

		case DLL_THREAD_DETACH:
			break;

		case DLL_PROCESS_DETACH:
			FreeConsole();
			delete mylog;
			delete myscript;
			free(script_buffer);
			break;
    }

    return TRUE;
}