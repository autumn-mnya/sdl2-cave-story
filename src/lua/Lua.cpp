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


#include "../rendering.h"
#include "../game_hooks.h"
#include "../API_Pause.h"
#include "../SDL_Pause.h"
#include <doukutsu/draw.h>
#include "../AutPI.h"

#define gL GetLuaL()
#define FUNCTION_TABLE_SDL_SIZE 4

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

static int lua_PutRectAlpha(lua_State* L)
{
	RECT* rect = (RECT*)luaL_checkudata(L, 1, "RectMeta");
	int x = (int)luaL_checknumber(L, 2);
	int y = (int)luaL_checknumber(L, 3);
	int surface = (int)luaL_checknumber(L, 4);
	int alpha = (int)luaL_checknumber(L, 5);

	PutBitmap3Alpha(&csvanilla::grcFull, x, y, rect, surface, alpha);

	return 0;
}

FUNCTION_TABLE SdlFunctionTable[FUNCTION_TABLE_SDL_SIZE] =
{
	{"GetFPS", lua_GetCurrentFPS},
	{"SetFPS", lua_SetCurrentFPS},
	{"ResetFPS", lua_ResetCurrentFPS},
	{"PutAlpha",lua_PutRectAlpha}
};

void PushLuaSDLFunctions()
{
	PushFunctionTable(gL, "SDL", SdlFunctionTable, FUNCTION_TABLE_SDL_SIZE, TRUE);
}