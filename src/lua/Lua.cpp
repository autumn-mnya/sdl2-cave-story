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
#define FUNCTION_TABLE_SDL_SIZE 5

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
	double angle = (double)luaL_optnumber(L, 6, 0);
	int r = (int)luaL_optnumber(L, 7, 0);
	int g = (int)luaL_optnumber(L, 8, 0);
	int b = (int)luaL_optnumber(L, 9, 0);

	PutBitmap3Alpha(&csvanilla::grcFull, x, y, rect, surface, alpha, angle, r, g, b);

	return 0;
}

static int lua_PutMat2x2(lua_State* L)
{
	RECT* rect = (RECT*)luaL_checkudata(L, 1, "RectMeta");
	float x0 = (float)luaL_checknumber(L, 2);
	float y0 = (float)luaL_checknumber(L, 3);
	float x1 = (float)luaL_checknumber(L, 4);
	float y1 = (float)luaL_checknumber(L, 5);
	float x2 = (float)luaL_checknumber(L, 6);
	float y2 = (float)luaL_checknumber(L, 7);
	float x3 = (float)luaL_checknumber(L, 8);
	float y3 = (float)luaL_checknumber(L, 9);
	int surf = (int)luaL_checknumber(L, 10);
	int color = (int)luaL_checknumber(L, 11);
	int alpha = (int)luaL_optnumber(L, 12, 255);
	PutMat2x2(rect, x0, y0, x1, y1, x2, y2, x3, y3, surf, color, alpha);
	return 0;
}

FUNCTION_TABLE SdlFunctionTable[FUNCTION_TABLE_SDL_SIZE] =
{
	{"GetFPS", lua_GetCurrentFPS},
	{"SetFPS", lua_SetCurrentFPS},
	{"ResetFPS", lua_ResetCurrentFPS},
	{"PutAlpha",lua_PutRectAlpha},
	{"PutMat2x2", lua_PutMat2x2},
};

void PushLuaSDLFunctions()
{
	PushFunctionTable(gL, "SDL", SdlFunctionTable, FUNCTION_TABLE_SDL_SIZE, TRUE);
}