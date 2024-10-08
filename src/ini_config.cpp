#include <Windows.h>
#include <cstdio>
#include <cstring>
#include "library/ini.h"
#include "library/INIReader.h"
#include <stdio.h>
#include <string>
#include "ini_config.h"

char cModsPath[MAX_PATH];
char cModulePath[MAX_PATH];

bool enable_60fps = false;
bool enable_font_option = true;
bool ignore_vanilla_gamepad_setting = true;
int background_water_level = 240;
bool ignore_verification_check = false;

void LoadConfig()
{
	char path[MAX_PATH];
	sprintf(path, "%s\\%s\\%s", cModsPath, "sdl", "settings.ini");

	FILE* fp = fopen(path, "r");
	if (!fp)
	{
		printf("sdl settings.ini file at path '%s' not found. Config not loaded.\n", path);
		return;
	}
	fclose(fp);

	INIReader main(path);

	ignore_verification_check = main.GetBoolean("Main", "Ignore Code Verification Check", false);
	enable_60fps = main.GetBoolean("Main", "Enable 60FPS", false);
	ignore_vanilla_gamepad_setting = main.GetBoolean("Main", "Ignore Vanilla Gamepad Setting", true);
	background_water_level = main.GetInteger("Main", "Background Water Level", 240);
	enable_font_option = main.GetBoolean("Pause Menu", "Enable Font Option", true);

	printf("Loaded sdl settings.ini\n");
}