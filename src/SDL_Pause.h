#pragma once

#include "Backend.h"
#include "LoadFont.h"

typedef struct CONFIG_BINDING
{
	int keyboard;
	unsigned char controller;
} CONFIG_BINDING;

enum
{
	BINDING_UP,
	BINDING_DOWN,
	BINDING_LEFT,
	BINDING_RIGHT,
	BINDING_ALT_UP,
	BINDING_ALT_DOWN,
	BINDING_ALT_LEFT,
	BINDING_ALT_RIGHT,
	BINDING_OK,
	BINDING_CANCEL,
	BINDING_JUMP,
	BINDING_SHOT,
	BINDING_ARMSREV,
	BINDING_ARMS,
	BINDING_ITEM,
	BINDING_MAP,
	BINDING_PAUSE,
	BINDING_SHIFT,
	BINDING_TOTAL
};

struct SDLCONFIG
{
	char proof[0x20];
	FONT_DATA font;
	unsigned char font_select;
	bool b60fps;
	CONFIG_BINDING bindings[BINDING_TOTAL];
};

extern CONFIG_BINDING bindings[BINDING_TOTAL];
extern bool gKeyboardState[BACKEND_KEYBOARD_TOTAL];

extern SDLCONFIG gSdlConfig;

BOOL LoadSDLConfigData(SDLCONFIG* conf);
BOOL SaveSDLConfigData(const SDLCONFIG* conf);
void DefaultSDLConfigData(SDLCONFIG* conf);
void SaveSdlConfig();
void InitSdlPauseMenuCalls();