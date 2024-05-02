#include <Windows.h>
#include "library/ini.h"
#include "library/INIReader.h"
#include <stdio.h>
#include <string>
#include "ini_config.h"

char cModsPath[MAX_PATH];
char cModulePath[MAX_PATH];

bool enable_60fps = false;
bool enable_pause_code = false;
bool enable_font_option = true;
bool ignore_vanilla_gamepad_setting = true;
int background_water_level = 240;
bool ignore_verification_check = false;

void LoadConfig()
{
	char path[MAX_PATH];
	sprintf(path, "%s\\%s", cModsPath, "sdl.ini");
	INIReader main(path);

	ignore_verification_check = main.GetBoolean("Main", "Ignore Code Verification Check", false);
	enable_60fps = main.GetBoolean("Main", "Enable 60FPS", false);
	enable_pause_code = main.GetBoolean("Main", "Enable Pause Code", false);
	ignore_vanilla_gamepad_setting = main.GetBoolean("Main", "Ignore Vanilla Gamepad Setting", true);
	background_water_level = main.GetInteger("Main", "Background Water Level", 240);
	enable_font_option = main.GetBoolean("Pause Menu", "Enable Font Option", true);

	printf("Loaded sdl.ini\n");
}