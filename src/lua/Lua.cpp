#include <Windows.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>

extern "C"
{
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
}

#include "Lua.h"


#include "../game_hooks.h"
#include "../API_Pause.h"
#include "../SDL_Pause.h"

#include "../AutPI.h"

#define gL GetLuaL()
#define FUNCTION_TABLE_SDL_SIZE 3

static int lua_GetCurrentFPS(lua_State* L)
{
	lua_pushnumber(L, gCurrentFPS);
	return 1;
}

static int lua_SetCurrentFPS(lua_State* L)
{
	int fps = (int)luaL_checknumber(L, 1);

	// dont allow fps to be 0 as it crashes the game
	if (fps == 0)
	{
		if (gCurrentFPS == -1)
			gCurrentFPS = 1;
	}
	else
		gCurrentFPS = fps;

	return 0;
}

static int lua_ResetCurrentFPS(lua_State* L)
{
	gCurrentFPS = -1;
	return 0;
}

FUNCTION_TABLE SdlFunctionTable[FUNCTION_TABLE_SDL_SIZE] =
{
	{"GetFPS", lua_GetCurrentFPS},
	{"SetFPS", lua_SetCurrentFPS},
	{"ResetFPS", lua_ResetCurrentFPS}
};

void PushLuaSDLFunctions()
{
	PushFunctionTable(gL, "SDL", SdlFunctionTable, FUNCTION_TABLE_SDL_SIZE, TRUE);
}