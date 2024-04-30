#pragma once

#include <Windows.h>

extern char cModsPath[MAX_PATH];
extern char cModulePath[MAX_PATH];

extern bool enable_60fps;
extern bool enable_pause_code;
extern bool enable_font_option;
extern bool ignore_vanilla_gamepad_setting;
extern int background_water_level;

void LoadConfig();