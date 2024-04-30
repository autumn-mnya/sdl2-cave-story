#pragma once

#include "LoadFont.h"
#include <vector>

struct DIRECTINPUTSTATUS
{
	BOOL bButton[128];	// The original `Input.cpp` assumed there were 32 buttons (because of DirectInput's `DIJOYSTATE` struct)
};

extern DIRECTINPUTSTATUS gJoystickState;

extern size_t font_width;
extern size_t font_height;

extern int mag;

void SwapFontObject(const FONT_DATA& font_data);