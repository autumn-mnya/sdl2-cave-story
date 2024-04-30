#include <Windows.h>
#include <iostream>
#include "API_Pause.h"
#include "ini_config.h"
#include "doukutsu/all.h"
#include "SDL_Pause.h"
#include "Backend.h"
#include "File.h"
#include "game_hooks.h"

const char* const gConfigName = "SDL.dat";
const char* const gProof = "SDL2M   20240428";

SDLCONFIG gSdlConfig;
CONFIG_BINDING bindings[BINDING_TOTAL];
bool gKeyboardState[BACKEND_KEYBOARD_TOTAL];

SDLCONFIG* GetSdlConf()
{
	return &gSdlConfig;
}

static const char* GetKeyName(int key)
{
	switch (key)
	{
	case BACKEND_KEYBOARD_A:
		return "A";

	case BACKEND_KEYBOARD_B:
		return "B";

	case BACKEND_KEYBOARD_C:
		return "C";

	case BACKEND_KEYBOARD_D:
		return "D";

	case BACKEND_KEYBOARD_E:
		return "E";

	case BACKEND_KEYBOARD_F:
		return "F";

	case BACKEND_KEYBOARD_G:
		return "G";

	case BACKEND_KEYBOARD_H:
		return "H";

	case BACKEND_KEYBOARD_I:
		return "I";

	case BACKEND_KEYBOARD_J:
		return "J";

	case BACKEND_KEYBOARD_K:
		return "K";

	case BACKEND_KEYBOARD_L:
		return "L";

	case BACKEND_KEYBOARD_M:
		return "M";

	case BACKEND_KEYBOARD_N:
		return "N";

	case BACKEND_KEYBOARD_O:
		return "O";

	case BACKEND_KEYBOARD_P:
		return "P";

	case BACKEND_KEYBOARD_Q:
		return "Q";

	case BACKEND_KEYBOARD_R:
		return "R";

	case BACKEND_KEYBOARD_S:
		return "S";

	case BACKEND_KEYBOARD_T:
		return "T";

	case BACKEND_KEYBOARD_U:
		return "U";

	case BACKEND_KEYBOARD_V:
		return "V";

	case BACKEND_KEYBOARD_W:
		return "W";

	case BACKEND_KEYBOARD_X:
		return "X";

	case BACKEND_KEYBOARD_Y:
		return "Y";

	case BACKEND_KEYBOARD_Z:
		return "Z";

	case BACKEND_KEYBOARD_0:
		return "0";

	case BACKEND_KEYBOARD_1:
		return "1";

	case BACKEND_KEYBOARD_2:
		return "2";

	case BACKEND_KEYBOARD_3:
		return "3";

	case BACKEND_KEYBOARD_4:
		return "4";

	case BACKEND_KEYBOARD_5:
		return "5";

	case BACKEND_KEYBOARD_6:
		return "6";

	case BACKEND_KEYBOARD_7:
		return "7";

	case BACKEND_KEYBOARD_8:
		return "8";

	case BACKEND_KEYBOARD_9:
		return "9";

	case BACKEND_KEYBOARD_F1:
		return "F1";

	case BACKEND_KEYBOARD_F2:
		return "F2";

	case BACKEND_KEYBOARD_F3:
		return "F3";

	case BACKEND_KEYBOARD_F4:
		return "F4";

	case BACKEND_KEYBOARD_F5:
		return "F5";

	case BACKEND_KEYBOARD_F6:
		return "F6";

	case BACKEND_KEYBOARD_F7:
		return "F7";

	case BACKEND_KEYBOARD_F8:
		return "F8";

	case BACKEND_KEYBOARD_F9:
		return "F9";

	case BACKEND_KEYBOARD_F10:
		return "F10";

	case BACKEND_KEYBOARD_F11:
		return "F11";

	case BACKEND_KEYBOARD_F12:
		return "F12";

	case BACKEND_KEYBOARD_UP:
		return "Up";

	case BACKEND_KEYBOARD_DOWN:
		return "Down";

	case BACKEND_KEYBOARD_LEFT:
		return "Left";

	case BACKEND_KEYBOARD_RIGHT:
		return "Right";

	case BACKEND_KEYBOARD_ESCAPE:
		return "Escape";

	case BACKEND_KEYBOARD_BACK_QUOTE:
		return "`";

	case BACKEND_KEYBOARD_TAB:
		return "Tab";

	case BACKEND_KEYBOARD_CAPS_LOCK:
		return "Caps Lock";

	case BACKEND_KEYBOARD_LEFT_SHIFT:
		return "Left Shift";

	case BACKEND_KEYBOARD_LEFT_CTRL:
		return "Left Ctrl";

	case BACKEND_KEYBOARD_LEFT_ALT:
		return "Left Alt";

	case BACKEND_KEYBOARD_SPACE:
		return "Space";

	case BACKEND_KEYBOARD_RIGHT_ALT:
		return "Right Alt";

	case BACKEND_KEYBOARD_RIGHT_CTRL:
		return "Right Ctrl";

	case BACKEND_KEYBOARD_RIGHT_SHIFT:
		return "Right Shift";

	case BACKEND_KEYBOARD_ENTER:
		return "Enter";

	case BACKEND_KEYBOARD_BACKSPACE:
		return "Backspace";

	case BACKEND_KEYBOARD_MINUS:
		return "-";

	case BACKEND_KEYBOARD_EQUALS:
		return "=";

	case BACKEND_KEYBOARD_LEFT_BRACKET:
		return "[";

	case BACKEND_KEYBOARD_RIGHT_BRACKET:
		return "]";

	case BACKEND_KEYBOARD_BACK_SLASH:
		return "\\";

	case BACKEND_KEYBOARD_SEMICOLON:
		return ";";

	case BACKEND_KEYBOARD_APOSTROPHE:
		return "'";

	case BACKEND_KEYBOARD_COMMA:
		return ",";

	case BACKEND_KEYBOARD_PERIOD:
		return ".";

	case BACKEND_KEYBOARD_FORWARD_SLASH:
		return "/";
	}

	return "Unknown";
}

void DefaultSDLConfigData(SDLCONFIG* conf)
{
	// Clear old configuration data
	memset(conf, 0, sizeof(SDLCONFIG));

	strcpy(conf->proof, gProof);

	conf->b60fps = true;

	conf->font = GetFirstFontData(gFontData);

	// Default keyboard bindings
	conf->bindings[BINDING_UP].keyboard = BACKEND_KEYBOARD_UP;
	conf->bindings[BINDING_DOWN].keyboard = BACKEND_KEYBOARD_DOWN;
	conf->bindings[BINDING_LEFT].keyboard = BACKEND_KEYBOARD_LEFT;
	conf->bindings[BINDING_RIGHT].keyboard = BACKEND_KEYBOARD_RIGHT;
	conf->bindings[BINDING_ALT_UP].keyboard = BACKEND_KEYBOARD_I;
	conf->bindings[BINDING_ALT_DOWN].keyboard = BACKEND_KEYBOARD_K;
	conf->bindings[BINDING_ALT_LEFT].keyboard = BACKEND_KEYBOARD_J;
	conf->bindings[BINDING_ALT_RIGHT].keyboard = BACKEND_KEYBOARD_L;
	conf->bindings[BINDING_OK].keyboard = BACKEND_KEYBOARD_Z;
	conf->bindings[BINDING_CANCEL].keyboard = BACKEND_KEYBOARD_X;
	conf->bindings[BINDING_JUMP].keyboard = BACKEND_KEYBOARD_Z;
	conf->bindings[BINDING_SHOT].keyboard = BACKEND_KEYBOARD_X;
	conf->bindings[BINDING_ARMSREV].keyboard = BACKEND_KEYBOARD_A;
	conf->bindings[BINDING_ARMS].keyboard = BACKEND_KEYBOARD_S;
	conf->bindings[BINDING_ITEM].keyboard = BACKEND_KEYBOARD_Q;
	conf->bindings[BINDING_MAP].keyboard = BACKEND_KEYBOARD_W;
	conf->bindings[BINDING_PAUSE].keyboard = BACKEND_KEYBOARD_P;
	conf->bindings[BINDING_SHIFT].keyboard = BACKEND_KEYBOARD_LEFT_SHIFT;

	//Default controller bindings
	conf->bindings[BINDING_UP].controller = 23;
	conf->bindings[BINDING_DOWN].controller = 25;
	conf->bindings[BINDING_LEFT].controller = 26;
	conf->bindings[BINDING_RIGHT].controller = 24;
	conf->bindings[BINDING_ALT_UP].controller = 13;
	conf->bindings[BINDING_ALT_DOWN].controller = 14;
	conf->bindings[BINDING_ALT_LEFT].controller = 11;
	conf->bindings[BINDING_ALT_RIGHT].controller = 12;
	conf->bindings[BINDING_OK].controller = 0;
	conf->bindings[BINDING_CANCEL].controller = 1;
	conf->bindings[BINDING_JUMP].controller = 0;
	conf->bindings[BINDING_SHOT].controller = 2;
	conf->bindings[BINDING_ARMSREV].controller = 4;
	conf->bindings[BINDING_ARMS].controller = 5;
	conf->bindings[BINDING_ITEM].controller = 3;
	conf->bindings[BINDING_MAP].controller = 6;
	conf->bindings[BINDING_PAUSE].controller = 7;
	conf->bindings[BINDING_SHIFT].controller = 10;
}

BOOL LoadSDLConfigData(SDLCONFIG* conf)
{
	// Clear old configuration data
	memset(conf, 0, sizeof(SDLCONFIG));

	// Get path
	char path[MAX_PATH];
	sprintf(path, "%s\\%s\\%s", csvanilla::gDataPath, "Config", gConfigName);

	// Open file
	FILE* fp = fopen(path, "rb");
	if (fp == NULL)
		return FALSE;

	// Read the version id
	fread(conf->proof, sizeof(conf->proof), 1, fp);

	// Read the font data
	fread(&conf->font, sizeof(FONT_DATA), 1, fp);

	// Read font data
	conf->font_select = fgetc(fp);

	// Read framerate toggle
	conf->b60fps = fgetc(fp);

	// Read key-bindings
	for (size_t i = 0; i < BINDING_TOTAL; ++i)
	{
		conf->bindings[i].keyboard = File_ReadLE32(fp);
		// writeToLog("Loading SDL Config -- %s%d%s is: %s\n", "Keyboard Binding[", i, "]", GetKeyName(conf->bindings[i].keyboard));
		conf->bindings[i].controller = fgetc(fp);
	}

	// Close file
	fclose(fp);

	// Check if version is not correct, and return if it failed
	if (strcmp(conf->proof, gProof))
	{
		memset(conf, 0, sizeof(SDLCONFIG));
		return FALSE;
	}

	printf("%s", "sdl config loaded successfully\n");
	return TRUE;
}

BOOL SaveSDLConfigData(const SDLCONFIG* conf)
{
	// Get path
	char path[MAX_PATH];
	sprintf(path, "%s\\%s\\%s", csvanilla::gDataPath, "Config", gConfigName);

	// Open file
	FILE* fp = fopen(path, "wb");
	if (fp == NULL)
		return FALSE;

	// Write the version id
	fwrite(conf->proof, sizeof(conf->proof), 1, fp);

	// Write the font data
	fwrite(&conf->font, sizeof(FONT_DATA), 1, fp);

	// Write font data
	fputc(conf->font_select, fp);

	// Write framerate toggle
	fputc(conf->b60fps, fp);

	// Write key-bindings
	for (size_t i = 0; i < BINDING_TOTAL; ++i)
	{
		File_WriteLE32(conf->bindings[i].keyboard, fp);
		// writeToLog("Saving SDL Config -- %s%d%s is: %s\n", "Keyboard Binding[", i, "]", GetKeyName(conf->bindings[i].keyboard));
		fputc(conf->bindings[i].controller, fp);
	}


	// Close file
	fclose(fp);

	return TRUE;
}

static void PutTextCentred(int x, int y, const char* text, unsigned long color)
{
	size_t string_width = 0;
	size_t string_height = font_height / csvanilla::window_magnification;

	for (const char* text_pointer = text; *text_pointer != '\0';)
	{
		if (csvanilla::IsShiftJIS(*text_pointer))
		{
			text_pointer += 2;
			string_width += (font_width / csvanilla::window_magnification) * 2;
		}
		else
		{
			text_pointer += 1;
			string_width += (font_width / csvanilla::window_magnification);
		}
	}

	csvanilla::PutText(x - string_width / 2, y - string_height / 2, text, color);
}

///////////////////
// Controls menu //
///////////////////

typedef struct Control
{
	const char* name;
	size_t binding_index;
	unsigned char groups;
} Control;

// The bitfield on the right determines which 'group' the
// control belongs to - if two controls are in the same group,
// they cannot be bound to the same key.
static const Control controls[] = {
	{"Up",              BINDING_UP,     (1 << 0) | (1 << 1)},
	{"Down",            BINDING_DOWN,   (1 << 0) | (1 << 1)},
	{"Left",            BINDING_LEFT,   (1 << 0) | (1 << 1)},
	{"Right",           BINDING_RIGHT,  (1 << 0) | (1 << 1)},
	{"Alt-Up",          BINDING_ALT_UP,     (1 << 0) | (1 << 1)},
	{"Alt-Down",        BINDING_ALT_DOWN,   (1 << 0) | (1 << 1)},
	{"Alt-Left",        BINDING_ALT_LEFT,   (1 << 0) | (1 << 1)},
	{"Alt-Right",       BINDING_ALT_RIGHT,  (1 << 0) | (1 << 1)},
	{"OK",              BINDING_OK,      1 << 1},
	{"Cancel",          BINDING_CANCEL,  1 << 1},
	{"Jump",            BINDING_JUMP,    1 << 0},
	{"Shoot",           BINDING_SHOT,    1 << 0},
	{"Next Weapon",     BINDING_ARMS,    1 << 0},
	{"Previous Weapon", BINDING_ARMSREV, 1 << 0},
	{"Inventory",       BINDING_ITEM,    1 << 0},
	{"Map",             BINDING_MAP,     1 << 0},
	{"Pause",           BINDING_PAUSE,   1 << 0},
	{"Shift",           BINDING_SHIFT,   1 << 0},
};

static char bound_name_buffers[sizeof(controls) / sizeof(controls[0])][20];

#define WINDOW_WIDTH csvanilla::grcGame.right
#define WINDOW_HEIGHT csvanilla::grcGame.bottom

static int Callback_ControlsKeyboard_Rebind(OptionsMenu* parent_menu, size_t this_option, CallbackAction action)
{
	switch (action)
	{
	default:
		break;

	case ACTION_INIT:
		strncpy(bound_name_buffers[this_option], GetKeyName(bindings[controls[this_option].binding_index].keyboard), sizeof(bound_name_buffers[0]) - 1);
		break;

	case ACTION_OK:
		csvanilla::PlaySoundObject(5, 1);

		char timeout_string[2];
		timeout_string[1] = '\0';

		bool previous_keyboard_state[BACKEND_KEYBOARD_TOTAL];
		memcpy(previous_keyboard_state, gKeyboardState, sizeof(gKeyboardState));

		// Time-out and exit if the user takes too long (they probably don't want to rebind)
		for (unsigned int timeout = (60 * 5) - 1; timeout != 0; --timeout)
		{
			for (int scancode = 0; scancode < BACKEND_KEYBOARD_TOTAL; ++scancode)
			{
				// Wait for user to press a key
				if (!previous_keyboard_state[scancode] && gKeyboardState[scancode])
				{
					const char* key_name = GetKeyName(scancode);

					// If another control in the same group uses this key, swap them
					for (size_t other_option = 0; other_option < parent_menu->total_options; ++other_option)
					{
						if (other_option != this_option && controls[other_option].groups & controls[this_option].groups && bindings[controls[other_option].binding_index].keyboard == scancode)
						{
							bindings[controls[other_option].binding_index].keyboard = bindings[controls[this_option].binding_index].keyboard;
							memcpy(bound_name_buffers[other_option], bound_name_buffers[this_option], sizeof(bound_name_buffers[0]));
							break;
						}
					}

					// Otherwise just overwrite the selected control
					bindings[controls[this_option].binding_index].keyboard = scancode;
					strncpy(bound_name_buffers[this_option], key_name, sizeof(bound_name_buffers[0]) - 1);

					csvanilla::PlaySoundObject(18, 1);

					csvanilla::gKeyTrg = csvanilla::gKey = 0;	// Prevent weird input-ghosting by doing this
					return CALLBACK_CONTINUE;
				}
			}

			memcpy(previous_keyboard_state, gKeyboardState, sizeof(gKeyboardState));

			// Draw screen
			csvanilla::CortBox(&csvanilla::grcFull, 0x000000);

			const char* string = "Press a key to bind to this action:";
			PutTextCentred(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2 - 10, string, RGB(0xFF, 0xFF, 0xFF));
			PutTextCentred(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2 + 10, parent_menu->options[this_option].name, RGB(0xFF, 0xFF, 0xFF));

			timeout_string[0] = '0' + (timeout / 60) + 1;
			PutTextCentred(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2 + 60, timeout_string, RGB(0xFF, 0xFF, 0xFF));

			csvanilla::PutFramePerSecound();

			if (!csvanilla::Flip_SystemTask(csvanilla::ghWnd))
			{
				// Quit if window is closed
				return CALLBACK_EXIT;
			}
		}

		break;
	}

	return CALLBACK_CONTINUE;
}

static int Callback_ControlsKeyboard(OptionsMenu* parent_menu, size_t this_option, CallbackAction action)
{
	(void)parent_menu;

	if (action != ACTION_OK)
		return CALLBACK_CONTINUE;

	Option options[sizeof(controls) / sizeof(controls[0])];

	for (size_t i = 0; i < sizeof(controls) / sizeof(controls[0]); ++i)
	{
		options[i].name = controls[i].name;
		options[i].callback = Callback_ControlsKeyboard_Rebind;
		options[i].value_string = bound_name_buffers[i];
		options[i].disabled = FALSE;
	}

	OptionsMenu options_menu = {
		"CONTROLS (KEYBOARD)",
		NULL,
		options,
		sizeof(options) / sizeof(options[0]),
		-60,
		TRUE
	};

	csvanilla::PlaySoundObject(5, 1);

	const int return_value = EnterOptionsMenu(&options_menu, 0);

	csvanilla::PlaySoundObject(5, 1);

	return return_value;
}

static int Callback_ControlsController_Rebind(OptionsMenu* parent_menu, size_t this_option, CallbackAction action)
{
	switch (action)
	{
	default:
		break;

	case ACTION_INIT:
		// Name each bound button
		if (bindings[controls[this_option].binding_index].controller == 0xFF)
			strncpy(bound_name_buffers[this_option], "[Unbound]", sizeof(bound_name_buffers[0]));
		else
			strncpy(bound_name_buffers[this_option], ControllerBackend_GetButtonName(bindings[controls[this_option].binding_index].controller), sizeof(bound_name_buffers[0]));

		break;

	case ACTION_OK:
		csvanilla::PlaySoundObject(5, 1);

		DIRECTINPUTSTATUS old_state = gJoystickState;

		char timeout_string[2];
		timeout_string[1] = '\0';

		// Time-out and exit if the user takes too long (they probably don't want to rebind)
		for (unsigned int timeout = (60 * 5) - 1; timeout != 0; --timeout)
		{
			for (int button = 0; button < sizeof(gJoystickState.bButton) / sizeof(gJoystickState.bButton[0]); ++button)
			{
				// Wait for user to press a button
				if (!old_state.bButton[button] && gJoystickState.bButton[button])
				{
					// If another control in the same group uses this button, swap them
					for (size_t other_option = 0; other_option < parent_menu->total_options; ++other_option)
					{
						if (other_option != this_option && controls[other_option].groups & controls[this_option].groups && bindings[controls[other_option].binding_index].controller == button)
						{
							bindings[controls[other_option].binding_index].controller = bindings[controls[this_option].binding_index].controller;
							memcpy(bound_name_buffers[other_option], bound_name_buffers[this_option], sizeof(bound_name_buffers[0]));
							break;
						}
					}

					// Otherwise just overwrite the selected control
					bindings[controls[this_option].binding_index].controller = button;
					strncpy(bound_name_buffers[this_option], ControllerBackend_GetButtonName(button), sizeof(bound_name_buffers[0]));

					csvanilla::PlaySoundObject(18, 1);

					csvanilla::gKeyTrg = csvanilla::gKey = 0;	// Prevent weird input-ghosting by doing this
					return CALLBACK_CONTINUE;
				}
			}

			old_state = gJoystickState;

			// Draw screen
			csvanilla::CortBox(&csvanilla::grcFull, 0x000000);

			const char* string = "Press a button to bind to this action:";
			PutTextCentred(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2 - 10, string, RGB(0xFF, 0xFF, 0xFF));
			PutTextCentred(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2 + 10, parent_menu->options[this_option].name, RGB(0xFF, 0xFF, 0xFF));

			timeout_string[0] = '0' + (timeout / 60) + 1;
			PutTextCentred(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2 + 60, timeout_string, RGB(0xFF, 0xFF, 0xFF));

			csvanilla::PutFramePerSecound();

			if (!csvanilla::Flip_SystemTask(csvanilla::ghWnd))
			{
				// Quit if window is closed
				return CALLBACK_EXIT;
			}
		}

		break;
	}

	return CALLBACK_CONTINUE;
}

static int Callback_ControlsController(OptionsMenu* parent_menu, size_t this_option, CallbackAction action)
{
	(void)parent_menu;

	if (action != ACTION_OK)
		return CALLBACK_CONTINUE;

	Option options[sizeof(controls) / sizeof(controls[0])];

	for (size_t i = 0; i < sizeof(controls) / sizeof(controls[0]); ++i)
	{
		options[i].name = controls[i].name;
		options[i].callback = Callback_ControlsController_Rebind;
		options[i].value_string = bound_name_buffers[i];
		options[i].disabled = FALSE;
	}

	OptionsMenu options_menu = {
	#if !defined(__WIIU__) && !defined(__3DS__)
		"CONTROLS (GAMEPAD)",
	#else
		"CONTROLS",
	#endif
		NULL,
		options,
		sizeof(options) / sizeof(options[0]),
		-70,
		TRUE
	};

	csvanilla::PlaySoundObject(5, 1);

	const int return_value = EnterOptionsMenu(&options_menu, 0);

	csvanilla::PlaySoundObject(5, 1);

	return return_value;
}

static int Callback_Framerate(OptionsMenu* parent_menu, size_t this_option, CallbackAction action)
{
	SDLCONFIG* conf = (SDLCONFIG*)parent_menu->options[this_option].user_data;

	const char* strings[] = { "50FPS", "60FPS" };

	switch (action)
	{
	case ACTION_INIT:
		parent_menu->options[this_option].value = conf->b60fps;
		parent_menu->options[this_option].value_string = strings[conf->b60fps];
		break;

	case ACTION_DEINIT:
		conf->b60fps = parent_menu->options[this_option].value;
		break;

	case ACTION_OK:
	case ACTION_LEFT:
	case ACTION_RIGHT:
		// Increment value (with wrapping)
		parent_menu->options[this_option].value = (parent_menu->options[this_option].value + 1) % (sizeof(strings) / sizeof(strings[0]));

		enable_60fps = parent_menu->options[this_option].value;

		csvanilla::PlaySoundObject(4, 1);

		parent_menu->options[this_option].value_string = strings[parent_menu->options[this_option].value];
		break;

	case ACTION_UPDATE:
		break;
	}

	return CALLBACK_CONTINUE;
}

static int Callback_Font(OptionsMenu* parent_menu, size_t this_option, CallbackAction action)
{
	SDLCONFIG* conf = (SDLCONFIG*)parent_menu->options[this_option].user_data;

	switch (action)
	{
	case ACTION_INIT:
		// Initialize the font select option
		parent_menu->options[this_option].value = conf->font_select;
		parent_menu->options[this_option].attribute_size = gFontData.size();
		parent_menu->options[this_option].attribute_index = conf->font_select;
		parent_menu->options[this_option].value_string = gFontData[conf->font_select].name;
		break;

	case ACTION_DEINIT:
		// Update the font select option on deinitialization
		conf->font_select = parent_menu->options[this_option].value;
		break;

	case ACTION_LEFT:
		// Decrement value (with wrapping)
		parent_menu->options[this_option].value = (parent_menu->options[this_option].value - 1 + gFontData.size()) % gFontData.size();

		// Play sound for option change
		csvanilla::PlaySoundObject(1, 1);

		// Swap the font object based on the selected option
		SwapFontObject(gFontData[parent_menu->options[this_option].value]);
		conf->font = gFontData[parent_menu->options[this_option].value];

		// Update the option text to reflect the selected font
		parent_menu->options[this_option].value_string = gFontData[parent_menu->options[this_option].value].name;
		parent_menu->options[this_option].attribute_index = parent_menu->options[this_option].value;
		break;

	case ACTION_OK:
	case ACTION_RIGHT:
		// Increment value (with wrapping)
		parent_menu->options[this_option].value = (parent_menu->options[this_option].value + 1) % gFontData.size();

		// Play sound for option change
		csvanilla::PlaySoundObject(1, 1);

		// Swap the font object based on the selected option
		SwapFontObject(gFontData[parent_menu->options[this_option].value]);
		conf->font = gFontData[parent_menu->options[this_option].value];

		// Update the option text to reflect the selected font
		parent_menu->options[this_option].value_string = gFontData[parent_menu->options[this_option].value].name;
		parent_menu->options[this_option].attribute_index = parent_menu->options[this_option].value;
		break;


	case ACTION_UPDATE:
		// Handle update action if needed
		break;
	}

	return CALLBACK_CONTINUE;
}

void SaveSdlConfig()
{
	memcpy(&gSdlConfig.bindings, bindings, sizeof(bindings));
	SaveSDLConfigData(&gSdlConfig);
}

// If we load the pause menu dll, then we add stuff to the pause menu for SDL related things.
void InitSdlPauseMenuCalls()
{
	if (!LoadSDLConfigData(&gSdlConfig))
	{
		DefaultSDLConfigData(&gSdlConfig);
	}
	else
	{
		// Set config values if they exist here
		enable_60fps = gSdlConfig.b60fps;
		SwapFontObject(gSdlConfig.font);
	}

	memcpy(bindings, gSdlConfig.bindings, sizeof(bindings));

	add_pause_entry(GetOptionsMenu(), "Controls (Keyboard)", Callback_ControlsKeyboard, NULL, NULL, 0, FALSE, GetNumEntriesAddedMenu());
	add_pause_entry(GetOptionsMenu(), "Controls (Gamepad)", Callback_ControlsController, NULL, NULL, 0, FALSE, GetNumEntriesAddedMenu());
	add_pause_entry(GetOptionsMenu(), "Framerate", Callback_Framerate, GetSdlConf(), NULL, 0, FALSE, GetNumEntriesAddedMenu());
	if (enable_font_option)
		add_pause_entry(GetOptionsMenu(), "Font", Callback_Font, GetSdlConf(), NULL, 0, FALSE, GetNumEntriesAddedMenu());
}