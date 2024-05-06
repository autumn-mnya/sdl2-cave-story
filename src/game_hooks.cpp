#include "SDL.h"

#include "rendering.h"
#include "loadimage.h"
#include "font.h"
#include "patch_utils.h"
#include "doukutsu/credits.h"
#include "doukutsu/cstdlib.h"
#include "doukutsu/draw.h"
#include "doukutsu/keybinds.h"
#include "doukutsu/map.h"
#include "doukutsu/organya.h"
#include "doukutsu/tsc.h"
#include "doukutsu/window.h"
#include <Windows.h>
#include <synchapi.h>
#include <ShlObj.h>
#include <string>
#include <cstddef>
#include <filesystem>
#include "ini_config.h"
#include <shlwapi.h>

#include "AutPI.h"
#include "API_Pause.h"
#include "SDL_Pause.h"

#include "game_hooks.h"
#include "Backend.h"
#include "LoadFont.h"
#include <vector>
#include "AutumnFunc.h"
#include "File.h"

#include "lua/Lua.h"

int gCurrentFPS = -1;

size_t font_width;
size_t font_height;

void ResetCurFPS()
{
	gCurrentFPS = -1;
}

void LoadPath()
{
	// Get executable's path
	GetModuleFileNameA(NULL, cModulePath, MAX_PATH);
	PathRemoveFileSpecA(cModulePath);

	// Get path of the mods folder
	strcpy(cModsPath, cModulePath);
	strcat(cModsPath, "\\mods");
}

// Replaces the CreateMutexA call at 0x412472
bool WINAPI initGraphics(LPSECURITY_ATTRIBUTES atrb, BOOL owner, LPCSTR name)
{
	csvanilla::hMutex = CreateMutexA(atrb, owner, name);
	try
	{
		renderer = new RenderBackend();
	}
	catch (...)
	{
		return false;
	}

	csvanilla::bActive = TRUE; // Fixes rando and other mods using the run-when-unfocused patch
	return true;
}



// Replaces the window initialization code starting at 0x4126F3
bool setupWindow(int conf_display_mode)
{
	if (renderer == nullptr)
		return false;

	bool error = false;
	int mag = 0;
	// Allow for higher scaled windowed resolutions (by overwriting the 16 and 24 bit fullscreen options)
	mag = conf_display_mode;
	if (conf_display_mode == 0) // Fullscreen
	{
		mag = 4;
		csvanilla::bFullscreen = true;
	}
	csvanilla::windowWidth = 320 * mag;
	csvanilla::windowHeight = 240 * mag;
	if (!renderer->initWindow(csvanilla::windowWidth, csvanilla::windowHeight, conf_display_mode))
		error = true;

	// This code is faithful to the original Config.dat specifications
	/*
	switch (conf_display_mode)
	{
	// Windowed mode
	case 1:
	case 2:
		csvanilla::windowWidth = 320 * conf_display_mode;
		csvanilla::windowHeight = 240 * conf_display_mode;
		if (!renderer->initWindow(csvanilla::windowWidth, csvanilla::windowHeight, conf_display_mode))
			error = true;
		break;
	// Fullscreen
	case 0:
	case 3:
	case 4:
		csvanilla::windowWidth = 640;
		csvanilla::windowHeight = 480;
		if (!renderer->initWindow(csvanilla::windowWidth, csvanilla::windowHeight, 0))
			error = true;
		csvanilla::bFullscreen = true;
		break;
	}
	*/

	if (error)
	{
		delete renderer;
		renderer = nullptr;
	}
	return !error;
}

// Replaces the ReleaseMutex call when exiting from WinMain()
void WINAPI deInit(HANDLE mutex)
{
	ReleaseMutex(mutex);
	delete renderer;
	renderer = nullptr;
}

bool SystemTask();

// Do config here for 60fps or 50fps
BOOL Flip_SystemTask(HWND)
{
	// TODO - Not the original variable names
	static unsigned long timePrev;
	static unsigned long timeNow;

	const unsigned int frameDelays[3] = { 17, 16, 17 };
	static unsigned int frame;

	unsigned int delay = enable_60fps ? frameDelays[frame % 3] : 20;
	if (gCurrentFPS != -1) {
		delay = 1000 / gCurrentFPS;
	}
	++frame;

	if (renderer == nullptr)
		return FALSE;

	while (true)
	{
		if (!SystemTask())
			return FALSE;

		timeNow = SDL_GetTicks();
		if (timeNow >= timePrev + delay)
			break;

		SDL_Delay(1);
	}

	if (timeNow >= timePrev + 100)
		timePrev = timeNow;
	else
		timePrev += delay;

	renderer->drawScreen();

	if (renderer->restoreSurfaces())
	{
		csvanilla::RestoreStripper();
		csvanilla::RestoreMapName();
		csvanilla::RestoreTextScript();
	}

	return TRUE;
}

DIRECTINPUTSTATUS gJoystickState;

static int joystick_neutral_x = 0;
static int joystick_neutral_y = 0;

void InitSDLInputThings()
{
	using csvanilla::gKey;
	using namespace csvanilla::Key;

	csvanilla::gKeyOk = KEY_OK;
	csvanilla::gKeyCancel = KEY_CANCEL;
}

void ReleaseDirectInput(void)
{
	ControllerBackend_Deinit();
}

BOOL GetJoystickStatus(DIRECTINPUTSTATUS* status)
{
	const size_t button_limit = sizeof(status->bButton) / sizeof(status->bButton[0]);

	bool* buttons;
	unsigned int button_count;

	short* axes;
	unsigned int axis_count;

	if (!ControllerBackend_GetJoystickStatus(&buttons, &button_count, &axes, &axis_count))
		return FALSE;

	if (button_count > button_limit)
		button_count = button_limit;

	for (unsigned int i = 0; i < button_count; ++i)
		status->bButton[i] = buttons[i];

	for (unsigned int i = button_count; i < button_limit; ++i)
		status->bButton[i] = FALSE;

	return TRUE;
}

BOOL ResetJoystickStatus(void)
{
	bool* buttons;
	unsigned int button_count;

	short* axes;
	unsigned int axis_count;

	if (!ControllerBackend_GetJoystickStatus(&buttons, &button_count, &axes, &axis_count))
		return FALSE;

	joystick_neutral_x = 0;
	joystick_neutral_y = 0;

	if (axis_count >= 1)
		joystick_neutral_x = axes[0];

	if (axis_count >= 2)
		joystick_neutral_y = axes[1];

	return TRUE;
}

void JoystickProc(void)
{
	int i;
	static DIRECTINPUTSTATUS old_status;

	using csvanilla::gKey;
	using namespace csvanilla::Key;

	if (!GetJoystickStatus(&gJoystickState))
		memset(&gJoystickState, 0, sizeof(gJoystickState));

	// Set held buttons
	for (i = 0; i < sizeof(gJoystickState.bButton) / sizeof(gJoystickState.bButton[0]); ++i)
	{
		if (gJoystickState.bButton[i] && !old_status.bButton[i])
		{
			if (i == bindings[BINDING_MAP].controller)
				gKey |= KEY_MAP;

			if (i == bindings[BINDING_LEFT].controller)
				gKey |= KEY_LEFT;
			if (i == bindings[BINDING_RIGHT].controller)
				gKey |= KEY_RIGHT;
			if (i == bindings[BINDING_UP].controller)
				gKey |= KEY_UP;
			if (i == bindings[BINDING_DOWN].controller)
				gKey |= KEY_DOWN;

			if (i == bindings[BINDING_ALT_LEFT].controller)
				gKey |= KEY_LEFT;
			if (i == bindings[BINDING_ALT_RIGHT].controller)
				gKey |= KEY_RIGHT;
			if (i == bindings[BINDING_ALT_UP].controller)
				gKey |= KEY_UP;
			if (i == bindings[BINDING_ALT_DOWN].controller)
				gKey |= KEY_DOWN;

			if (i == bindings[BINDING_SHOT].controller)
				gKey |= KEY_X;
			if (i == bindings[BINDING_JUMP].controller)
				gKey |= KEY_Z;
			if (i == bindings[BINDING_ARMS].controller)
				gKey |= KEY_ARMS;
			if (i == bindings[BINDING_ARMSREV].controller)
				gKey |= KEY_ARMSREV;
			if (i == bindings[BINDING_ITEM].controller)
				gKey |= KEY_ITEM;
			if (i == bindings[BINDING_CANCEL].controller)
				gKey |= KEY_CANCEL;
			if (i == bindings[BINDING_OK].controller)
				gKey |= KEY_OK;
			if (i == bindings[BINDING_PAUSE].controller)
				gKey |= KEY_ESCAPE;
			if (i == bindings[BINDING_SHIFT].controller)
				gKey |= KEY_SHIFT;
		}
		else if (!gJoystickState.bButton[i] && old_status.bButton[i])
		{
			if (i == bindings[BINDING_MAP].controller)
				gKey &= ~KEY_MAP;

			if (i == bindings[BINDING_LEFT].controller)
				gKey &= ~KEY_LEFT;
			if (i == bindings[BINDING_RIGHT].controller)
				gKey &= ~KEY_RIGHT;
			if (i == bindings[BINDING_UP].controller)
				gKey &= ~KEY_UP;
			if (i == bindings[BINDING_DOWN].controller)
				gKey &= ~KEY_DOWN;

			if (i == bindings[BINDING_ALT_LEFT].controller)
				gKey &= ~KEY_LEFT;
			if (i == bindings[BINDING_ALT_RIGHT].controller)
				gKey &= ~KEY_RIGHT;
			if (i == bindings[BINDING_ALT_UP].controller)
				gKey &= ~KEY_UP;
			if (i == bindings[BINDING_ALT_DOWN].controller)
				gKey &= ~KEY_DOWN;

			if (i == bindings[BINDING_SHOT].controller)
				gKey &= ~KEY_X;
			if (i == bindings[BINDING_JUMP].controller)
				gKey &= ~KEY_Z;
			if (i == bindings[BINDING_ARMS].controller)
				gKey &= ~KEY_ARMS;
			if (i == bindings[BINDING_ARMSREV].controller)
				gKey &= ~KEY_ARMSREV;
			if (i == bindings[BINDING_ITEM].controller)
				gKey &= ~KEY_ITEM;
			if (i == bindings[BINDING_CANCEL].controller)
				gKey &= ~KEY_CANCEL;
			if (i == bindings[BINDING_OK].controller)
				gKey &= ~KEY_OK;
			if (i == bindings[BINDING_PAUSE].controller)
				gKey &= ~KEY_ESCAPE;
			if (i == bindings[BINDING_SHIFT].controller)
				gKey &= ~KEY_SHIFT;
		}
	}

	old_status = gJoystickState;
}

BOOL InitDirectInput(void)
{
	return ControllerBackend_Init();
}

void InitInput()
{
	InitDirectInput();
}

void RegisterSDLInputHooks()
{
	RegisterPreModeElement(InitSDLInputThings);
	RegisterPreModeElement(InitInput);
	RegisterReleaseElement(ReleaseDirectInput);
}

bool SystemTask()
{
	using csvanilla::bActive;
	static bool previous_keyboard_state[BACKEND_KEYBOARD_TOTAL];

	do
	{
		if (!Backend_SystemTask(bActive))
		{
			csvanilla::StopOrganyaMusic();
			return FALSE;
		}
	} while (!bActive);

	using csvanilla::gKey;
	using namespace csvanilla::Key;

	Backend_GetKeyboardState(gKeyboardState);

	for (unsigned int i = 0; i < BACKEND_KEYBOARD_TOTAL; ++i)
	{
		if (gKeyboardState[i] && !previous_keyboard_state[i])
		{
			if (i == BACKEND_KEYBOARD_ESCAPE)
				gKey |= KEY_ESCAPE;
			else if (i == BACKEND_KEYBOARD_F1)
				gKey |= KEY_F1;
			else if (i == BACKEND_KEYBOARD_F2)
				gKey |= KEY_F2;

			if (i == bindings[BINDING_MAP].keyboard)
				gKey |= KEY_MAP;

			if (i == bindings[BINDING_LEFT].keyboard)
				gKey |= KEY_LEFT;
			if (i == bindings[BINDING_RIGHT].keyboard)
				gKey |= KEY_RIGHT;
			if (i == bindings[BINDING_UP].keyboard)
				gKey |= KEY_UP;
			if (i == bindings[BINDING_DOWN].keyboard)
				gKey |= KEY_DOWN;

			if (i == bindings[BINDING_ALT_LEFT].keyboard)
				gKey |= KEY_LEFT;
			if (i == bindings[BINDING_ALT_RIGHT].keyboard)
				gKey |= KEY_RIGHT;
			if (i == bindings[BINDING_ALT_UP].keyboard)
				gKey |= KEY_UP;
			if (i == bindings[BINDING_ALT_DOWN].keyboard)
				gKey |= KEY_DOWN;

			if (i == bindings[BINDING_SHOT].keyboard)
				gKey |= KEY_X;
			if (i == bindings[BINDING_JUMP].keyboard)
				gKey |= KEY_Z;
			if (i == bindings[BINDING_ARMS].keyboard)
				gKey |= KEY_ARMS;
			if (i == bindings[BINDING_ARMSREV].keyboard)
				gKey |= KEY_ARMSREV;
			if (i == bindings[BINDING_ITEM].keyboard)
				gKey |= KEY_ITEM;
			if (i == bindings[BINDING_CANCEL].keyboard)
				gKey |= KEY_CANCEL;
			if (i == bindings[BINDING_OK].keyboard)
				gKey |= KEY_OK;
			if (i == bindings[BINDING_PAUSE].keyboard)
				gKey |= KEY_ESCAPE;
			if (i == bindings[BINDING_SHIFT].keyboard)
				gKey |= KEY_SHIFT;
		}
		else if (!gKeyboardState[i] && previous_keyboard_state[i])
		{
			if (i == BACKEND_KEYBOARD_ESCAPE)
				gKey &= ~KEY_ESCAPE;
			else if (i == BACKEND_KEYBOARD_F1)
				gKey &= ~KEY_F1;
			else if (i == BACKEND_KEYBOARD_F2)
				gKey &= ~KEY_F2;

			if (i == bindings[BINDING_MAP].keyboard)
				gKey &= ~KEY_MAP;

			if (i == bindings[BINDING_LEFT].keyboard)
				gKey &= ~KEY_LEFT;
			if (i == bindings[BINDING_RIGHT].keyboard)
				gKey &= ~KEY_RIGHT;
			if (i == bindings[BINDING_UP].keyboard)
				gKey &= ~KEY_UP;
			if (i == bindings[BINDING_DOWN].keyboard)
				gKey &= ~KEY_DOWN;

			if (i == bindings[BINDING_ALT_LEFT].keyboard)
				gKey &= ~KEY_LEFT;
			if (i == bindings[BINDING_ALT_RIGHT].keyboard)
				gKey &= ~KEY_RIGHT;
			if (i == bindings[BINDING_ALT_UP].keyboard)
				gKey &= ~KEY_UP;
			if (i == bindings[BINDING_ALT_DOWN].keyboard)
				gKey &= ~KEY_DOWN;

			if (i == bindings[BINDING_SHOT].keyboard)
				gKey &= ~KEY_X;
			if (i == bindings[BINDING_JUMP].keyboard)
				gKey &= ~KEY_Z;
			if (i == bindings[BINDING_ARMS].keyboard)
				gKey &= ~KEY_ARMS;
			if (i == bindings[BINDING_ARMSREV].keyboard)
				gKey &= ~KEY_ARMSREV;
			if (i == bindings[BINDING_ITEM].keyboard)
				gKey &= ~KEY_ITEM;
			if (i == bindings[BINDING_CANCEL].keyboard)
				gKey &= ~KEY_CANCEL;
			if (i == bindings[BINDING_OK].keyboard)
				gKey &= ~KEY_OK;
			if (i == bindings[BINDING_PAUSE].keyboard)
				gKey &= ~KEY_ESCAPE;
			if (i == bindings[BINDING_SHIFT].keyboard)
				gKey &= ~KEY_SHIFT;
		}
	}

	memcpy(previous_keyboard_state, gKeyboardState, sizeof(gKeyboardState));

	if (ignore_vanilla_gamepad_setting || csvanilla::gbUseJoystick)
		JoystickProc();

	
	return true;
}

void ReleaseSurface(int surf_no)
{
	if (renderer != nullptr)
	{
		renderer->releaseSurface(surf_no);
		csvanilla::memset(&csvanilla::surface_metadata[surf_no], 0, sizeof(csvanilla::SurfaceMetadata));
	}
}

bool has_loaded_fontyaml = false;
// Load the yaml file for fonts
void InitTextYaml()
{
	char path[MAX_PATH];
	sprintf(path, "%s\\%s\\%s", csvanilla::gDataPath, "Font", "font.list.yaml");
	gFontData = LoadFontDataFromYAML(path);
}

// Modified by autumn to load a font like CSE2, from the data\\Font folder.
void InitTextObject(const char* name)
{
	if (has_loaded_fontyaml == false)
	{
		InitTextYaml();
		has_loaded_fontyaml = true;
	}

	FONT_DATA boot_font = GetFirstFontData(gFontData);
	char path[MAX_PATH];

	int mag = csvanilla::window_magnification;

	font_width = boot_font.x_scaling * mag;
	font_height = boot_font.y_scaling * mag;

	sprintf(path, "%s\\%s\\%s", csvanilla::gDataPath, "Font", boot_font.filename);
	font = LoadFreeTypeFont(path, font_width, font_height);
}

void SwapFontObject(const FONT_DATA& font_data)
{
	UnloadFont(font);
	char path[MAX_PATH];

	// Construct the path to the font file
	sprintf(path, "%s\\%s\\%s", csvanilla::gDataPath, "Font", font_data.filename);
	// Calculate font width and height
	int mag = csvanilla::window_magnification;
	font_width = font_data.x_scaling * mag;
	font_height = font_data.y_scaling * mag;

	// Load the new font
	font = LoadFreeTypeFont(path, font_width, font_height);
}


void PutText(int x, int y, const char* text, unsigned long color)
{
	if (renderer == nullptr)
		return;
	
	int mag = csvanilla::window_magnification;
	renderer->drawText(font, RenderBackend::FramebufferID, x * mag, y * mag, color, text);
}
void PutText2(int x, int y, const char* text, unsigned long color, int surf_no)
{
	if (renderer == nullptr)
		return;

	int mag = csvanilla::window_magnification;
	renderer->drawText(font, surf_no, x * mag, y * mag, color, text);
}

void EndTextObject()
{
	UnloadFont(font);
}

static unsigned long color_black;

// Replacement for InitBack() so that we can load pngs
BOOL InitBack(const char* fName, int type)
{
	char path[MAX_PATH];
	FILE* fp;

	color_black = GetCortBoxColor(RGB(0, 0, 0x10));	// Unused. This may have once been used by background type 4 (the solid black background)

	// Get width and height
	fp = NULL;
	const char* bmp_file_extensions[] = { "pbm", "bmp" };
	for (size_t i = 0; i < sizeof(bmp_file_extensions) / sizeof(bmp_file_extensions[0]) && fp == NULL; ++i)
	{
		sprintf(path, "%s\\%s.%s", csvanilla::gDataPath, fName, bmp_file_extensions[i]);
		fp = fopen(path, "rb");
	}

	if (fp != NULL)
	{
		if (fgetc(fp) != 'B' || fgetc(fp) != 'M')
		{
#ifdef FIX_MAJOR_BUGS
			// The original game forgets to close fp
			fclose(fp);
#endif
			return FALSE;
		}

		fseek(fp, 18, SEEK_SET);

		csvanilla::gBack.partsW = File_ReadLE32(fp);
		csvanilla::gBack.partsH = File_ReadLE32(fp);
		fclose(fp);
	}
	else
	{
		sprintf(path, "%s\\%s.%s", csvanilla::gDataPath, fName, "png");
		fp = fopen(path, "rb");

		if (fp == NULL)
			return FALSE;

		if (fgetc(fp) != 0x89 || fgetc(fp) != 'P' || fgetc(fp) != 'N' || fgetc(fp) != 'G')
		{
			fclose(fp);
			return FALSE;
		}

		fseek(fp, 16, SEEK_SET);

		csvanilla::gBack.partsW = File_ReadBE32(fp);
		csvanilla::gBack.partsH = File_ReadBE32(fp);
		fclose(fp);
	}

	csvanilla::gBack.partsW /= 1;
	csvanilla::gBack.partsH /= 1;

	csvanilla::gBack.flag = TRUE;	// This variable is otherwise unused

	// *Now* we actually load the bitmap
	csvanilla::ReleaseSurface(0x1C);
	csvanilla::MakeSurface_File(fName, 0x1C);

	csvanilla::gBack.type = type;
	csvanilla::gWaterY = background_water_level * 0x10 * 0x200;
	return TRUE;
}

#define VERIFY(addr) if (!patcher::verifyBytes(0x##addr, origBytes##addr, sizeof origBytes##addr)) return false

// Verify that the memory addresses that we're patching contain the original code
// (i.e., haven't been touched by another ASM or DLL hack)
bool verifyIntegrity()
{
	const patcher::byte origBytes412472[] = {0xFF, 0x15, 0xC4, 0xC0, 0x48, 0x00, 0xA3, 0x78, 0xE4, 0x49, 0x00, 0x8B, 0x45, 0x08};
	VERIFY(412472);

	const patcher::byte origBytes4126F3[] = {0x6A, 0x30, 0x6A, 0x00, 0x8D, 0x45, 0xA8, 0x50, 0xE8, 0x30, 0xE6, 0x06, 0x00, 0x83,
		0xC4, 0x0C, 0xC7, 0x45, 0xA8, 0x30, 0x00, 0x00, 0x00, 0xC7, 0x45, 0xB0, 0xA0, 0x2C, 0x41, 0x00, 0x8B, 0x4D, 0x08};
	VERIFY(4126F3);

	const patcher::byte callReleaseMutex[] = {0xFF, 0x15, 0xC8, 0xC0, 0x48, 0x00};
	if (!patcher::verifyBytes(0x412AF3, callReleaseMutex, sizeof callReleaseMutex) ||
		!patcher::verifyBytes(0x412B78, callReleaseMutex, sizeof callReleaseMutex))
		return false;

	const patcher::byte origBytes412B20[] = {0xE8, 0x8B, 0xF3, 0xFF, 0xFF};
	VERIFY(412B20);

	const patcher::byte origBytes412B69[] = {0xE8, 0x52, 0x8B, 0xFF, 0xFF};
	VERIFY(412B69);

	auto verifyFunctionHeader = [](patcher::dword address)
	{
		const patcher::byte header[] = {0x55, 0x8B, 0xEC};
		return patcher::verifyBytes(address, header, sizeof header);
	};

	const patcher::dword replacedFunctions[] = {
		reinterpret_cast<patcher::dword>(csvanilla::Flip_SystemTask),
		reinterpret_cast<patcher::dword>(csvanilla::ReleaseSurface),
		reinterpret_cast<patcher::dword>(csvanilla::MakeSurface_Resource),
		reinterpret_cast<patcher::dword>(csvanilla::MakeSurface_File),
		reinterpret_cast<patcher::dword>(csvanilla::ReloadBitmap_Resource),
		reinterpret_cast<patcher::dword>(csvanilla::ReloadBitmap_File),
		reinterpret_cast<patcher::dword>(csvanilla::MakeSurface_Generic),
		reinterpret_cast<patcher::dword>(csvanilla::BackupSurface),
		reinterpret_cast<patcher::dword>(csvanilla::PutBitmap3),
		reinterpret_cast<patcher::dword>(csvanilla::PutBitmap4),
		reinterpret_cast<patcher::dword>(csvanilla::Surface2Surface),
		reinterpret_cast<patcher::dword>(csvanilla::GetCortBoxColor),
		reinterpret_cast<patcher::dword>(csvanilla::CortBox),
		reinterpret_cast<patcher::dword>(csvanilla::CortBox2),
		reinterpret_cast<patcher::dword>(csvanilla::RestoreSurfaces),
		reinterpret_cast<patcher::dword>(csvanilla::InitTextObject),
		reinterpret_cast<patcher::dword>(csvanilla::PutText),
		reinterpret_cast<patcher::dword>(csvanilla::PutText2),
		reinterpret_cast<patcher::dword>(csvanilla::EndTextObject),
		reinterpret_cast<patcher::dword>(csvanilla::InitBack),
	};
	for (patcher::dword func : replacedFunctions)
		if (!verifyFunctionHeader(func))
			return false;

	return true;
}

// Called upon DLL initialization
bool applySDLPatches()
{
	LoadPath(); // load mods folder path - autumn
	LoadConfig(); // load config file - autumn

	if (enable_pause_code)
	{
		LoadAutPiDll();
		LoadPauseMenuDll();
		RegisterPreModeElement(InitSdlPauseMenuCalls);
		RegisterSaveConfigElement(SaveSdlConfig);
	}

	RegisterSDLInputHooks();

	// Enable lua functions
	RegisterOpeningInitElement(ResetCurFPS);
	RegisterInitElement(ResetCurFPS);
	RegisterLuaFuncElement(PushLuaSDLFunctions);

	// Verify all bytes that we're overwriting, before doing anything

	if (ignore_verification_check == false)
	{
		if (!verifyIntegrity())
			return false;
	}

	// Initialize SDL at start of WinMain()
	const patcher::byte initGraphicsReturnValueChk[] = {
		0x84, 0xC0,                         // test al, al
		0x0F, 0x84, 0xF2, 0x06, 0x00, 0x00, // jz 412B71 ; ReleaseMutex(), end of WinMain
		0x90
	};
	patcher::writeCall(0x412472, initGraphics);
	patcher::patchBytes(0x412477, initGraphicsReturnValueChk, sizeof initGraphicsReturnValueChk);

	// Redirect window and DirectDraw initialization calls to our code
	const patcher::byte initWindow[] = {
		0xFF, 0xB5, 0x74, 0xFF, 0xFF, 0xFF, // push dword ptr [ebp-8c] ; conf.display_mode
		0xE8, 0x00, 0x00, 0x00, 0x00,       // call (our code)
		0x59,                               // pop ecx
		0x84, 0xC0,                         // test al, al
		0x0F, 0x84, 0x6A, 0x04, 0x00, 0x00, // jz 412b71 ; ReleaseMutex(), end of WinMain
		0xA1, 0x58, 0xE4, 0x49, 0x00,       // mov eax, dword ptr [49e458] ; ghWnd
		0x89, 0x45, 0xE8,                   // mov dword ptr [ebp-18], eax ; hWnd
		0xE9, 0x1D, 0x03, 0x00, 0x00        // jmp 412a31 ; jump past vanilla windows initialization code
	};
	patcher::patchBytes(0x4126F3, initWindow, sizeof initWindow);
	patcher::writeCall(0x4126F9, setupWindow);

	// nop out EndDirectDraw call (this is handled by deInit later)
	patcher::writeNOPs(0x412B69, 5);

	// Release all resources on exit
	patcher::writeCall(0x412AF3, deInit);
	patcher::writeNOPs(0x412AF8, 1);

	patcher::writeCall(0x412B78, deInit);
	patcher::writeNOPs(0x412B7D, 1);

	// Replace drawing functions
	patcher::replaceFunction(csvanilla::Flip_SystemTask, Flip_SystemTask);
	patcher::replaceFunction(csvanilla::ReleaseSurface, ReleaseSurface);
	patcher::replaceFunction(csvanilla::MakeSurface_Resource, MakeSurface_Resource);
	patcher::replaceFunction(csvanilla::MakeSurface_File, MakeSurface_File);
	patcher::replaceFunction(csvanilla::ReloadBitmap_Resource, ReloadBitmap_Resource);
	patcher::replaceFunction(csvanilla::ReloadBitmap_File, ReloadBitmap_File);
	patcher::replaceFunction(csvanilla::MakeSurface_Generic, MakeSurface_Generic);
	patcher::replaceFunction(csvanilla::BackupSurface, BackupSurface);
	patcher::replaceFunction(csvanilla::PutBitmap3, PutBitmap3);
	patcher::replaceFunction(csvanilla::PutBitmap4, PutBitmap4);
	patcher::replaceFunction(csvanilla::Surface2Surface, Surface2Surface);
	patcher::replaceFunction(csvanilla::GetCortBoxColor, GetCortBoxColor);
	patcher::replaceFunction(csvanilla::CortBox, CortBox);
	patcher::replaceFunction(csvanilla::CortBox2, CortBox2);
	patcher::replaceFunction(csvanilla::RestoreSurfaces, RestoreSurfaces);
	patcher::replaceFunction(csvanilla::InitTextObject, InitTextObject);
	patcher::replaceFunction(csvanilla::PutText, PutText);
	patcher::replaceFunction(csvanilla::PutText2, PutText2);
	patcher::replaceFunction(csvanilla::EndTextObject, EndTextObject);
	patcher::replaceFunction(csvanilla::InitBack, InitBack);

	return true;
}
