#include "Attributes.h"

#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <sys/stat.h>

#include "doukutsu/audio.h"
#include "doukutsu/organya.h"
#include "doukutsu/sound.h"
#include "doukutsu/window.h"
#include "Backend.h"
#include "rendering.h"
#include "SDL.h"

#define DEADZONE 10000
static SDL_Joystick* joystick;
static Sint16* axis_neutrals;

ATTRIBUTE_FORMAT_PRINTF(1, 2) void Backend_PrintInfo(const char* format, ...)
{
	va_list argumentList;
	va_start(argumentList, format);
	fputs("INFO: ", stdout);
	vfprintf(stdout, format, argumentList);
	fputc('\n', stdout);
	va_end(argumentList);
}

ATTRIBUTE_FORMAT_PRINTF(1, 2) void Backend_PrintError(const char* format, ...)
{
	va_list argumentList;
	va_start(argumentList, format);
	fputs("ERROR: ", stderr);
	vfprintf(stderr, format, argumentList);
	fputc('\n', stderr);
	va_end(argumentList);
}

bool ControllerBackend_Init(void)
{
	if (SDL_InitSubSystem(SDL_INIT_JOYSTICK) < 0)
	{
		Backend_PrintError("Couldn't initialise joystick SDL subsystem: %s", SDL_GetError());
		return false;
	}

#if !SDL_VERSION_ATLEAST(2, 0, 0)
	if (SDL_NumJoysticks() > 0)
		ControllerBackend_JoystickConnect(0);
#endif

	// Connect the controller if we're on Nintendo Switch
#ifdef SWITCH
	ControllerBackend_JoystickConnect(0);
#endif

	return true;
}

void ControllerBackend_Deinit(void)
{
	if (joystick != NULL)
	{
		SDL_JoystickClose(joystick);
		joystick = NULL;
	}

	SDL_QuitSubSystem(SDL_INIT_JOYSTICK);
}

bool ControllerBackend_GetJoystickStatus(bool** buttons, unsigned int* button_count, short** axes, unsigned int* axis_count)
{
	if (joystick == NULL)
		return false;

	int total_sdl_buttons = SDL_JoystickNumButtons(joystick);
	if (total_sdl_buttons < 0)
	{
		total_sdl_buttons = 0;
		Backend_PrintError("Failed to get number of buttons on joystick: %s", SDL_GetError());
	}

	int total_sdl_axes = SDL_JoystickNumAxes(joystick);
	if (total_sdl_axes < 0)
	{
		total_sdl_axes = 0;
		Backend_PrintError("Failed to get number of general axis controls on joystick: %s", SDL_GetError());
	}

	int total_sdl_hats = SDL_JoystickNumHats(joystick);
	if (total_sdl_hats < 0)
	{
		total_sdl_hats = 0;
		Backend_PrintError("Failed to get number of POV hats on joystick: %s", SDL_GetError());
	}

	*button_count = total_sdl_buttons + total_sdl_axes * 2 + total_sdl_hats * 4;
	*axis_count = total_sdl_axes;

	static bool* button_buffer = NULL;
	static short* axis_buffer = NULL;

	bool* new_button_buffer = (bool*)realloc(button_buffer, *button_count * sizeof(bool));

	if (new_button_buffer == NULL)
		return false;

	button_buffer = new_button_buffer;

	short* new_axis_buffer = (short*)realloc(axis_buffer, *axis_count * sizeof(short));

	if (new_axis_buffer == NULL)
		return false;

	axis_buffer = new_axis_buffer;

	//////////////////////////
	// Handle button inputs //
	//////////////////////////

	unsigned int current_button = 0;

	// Start with the joystick buttons
	for (int i = 0; i < total_sdl_buttons; ++i)
		button_buffer[current_button++] = SDL_JoystickGetButton(joystick, i);

	// Then the joystick axes
	for (int i = 0; i < total_sdl_axes; ++i)
	{
		Sint16 axis = SDL_JoystickGetAxis(joystick, i);

		button_buffer[current_button++] = axis < axis_neutrals[i] - DEADZONE;
		button_buffer[current_button++] = axis > axis_neutrals[i] + DEADZONE;
	}

	// Then the joystick hats
	for (int i = 0; i < total_sdl_hats; ++i)
	{
		Uint8 hat = SDL_JoystickGetHat(joystick, i);

		button_buffer[current_button++] = hat == SDL_HAT_UP || hat == SDL_HAT_LEFTUP || hat == SDL_HAT_RIGHTUP;
		button_buffer[current_button++] = hat == SDL_HAT_RIGHT || hat == SDL_HAT_RIGHTUP || hat == SDL_HAT_RIGHTDOWN;
		button_buffer[current_button++] = hat == SDL_HAT_DOWN || hat == SDL_HAT_LEFTDOWN || hat == SDL_HAT_RIGHTDOWN;
		button_buffer[current_button++] = hat == SDL_HAT_LEFT || hat == SDL_HAT_LEFTUP || hat == SDL_HAT_LEFTDOWN;
	}

	*buttons = button_buffer;

	////////////////////////
	// Handle axis inputs //
	////////////////////////

	for (int i = 0; i < total_sdl_axes; ++i)
		axis_buffer[i] = SDL_JoystickGetAxis(joystick, i);

	*axes = axis_buffer;

	return true;
}

void ControllerBackend_JoystickConnect(Sint32 joystick_id)
{
#if SDL_VERSION_ATLEAST(2, 0, 0)
	const char* joystick_name = SDL_JoystickNameForIndex(joystick_id);
#else
	const char* joystick_name = SDL_JoystickName(joystick_id);
#endif

	if (joystick_name != NULL)
	{
		Backend_PrintInfo("Joystick #%d connected - %s", joystick_id, joystick_name);
	}
	else
	{
		Backend_PrintError("Couldn't get joystick name: %s", SDL_GetError());
		Backend_PrintInfo("Joystick #%d connected - Name unknown", joystick_id);
	}

	if (joystick == NULL)
	{
		joystick = SDL_JoystickOpen(joystick_id);

		if (joystick != NULL)
		{
			int total_axes = SDL_JoystickNumAxes(joystick);
			if (total_axes < 0)
				Backend_PrintError("Couldn't get number of general axis control on connected joystick: %s", SDL_GetError());

			int total_buttons = SDL_JoystickNumButtons(joystick);
			if (total_buttons < 0)
				Backend_PrintError("Couldn't get number of buttons on connected joystick: %s", SDL_GetError());

			if (total_buttons >= 6)
			{
				Backend_PrintInfo("Joystick #%d selected", joystick_id);

				// Set up neutral axes
				axis_neutrals = (Sint16*)malloc(sizeof(Sint16) * total_axes);

				if (axis_neutrals != NULL)
				{
					for (int i = 0; i < total_axes; ++i)
						axis_neutrals[i] = SDL_JoystickGetAxis(joystick, i);

					return;
				}
				else
				{
					Backend_PrintError("Couldn't allocate memory for neutral axes");
				}
			}

			SDL_JoystickClose(joystick);
			joystick = NULL;
		}
		else
		{
			Backend_PrintError("Couldn't open joystick for use: %s", SDL_GetError());
		}
	}
}

void ControllerBackend_JoystickDisconnect(Sint32 joystick_id)
{
#if SDL_VERSION_ATLEAST(2, 0, 0)
	SDL_JoystickID current_joystick_id = SDL_JoystickInstanceID(joystick);
	if (current_joystick_id < 0)
		Backend_PrintError("Couldn't get instance ID for current joystick: %s", SDL_GetError());

	if (joystick_id == current_joystick_id)
	{
		Backend_PrintInfo("Joystick #%d disconnected", joystick_id);
		SDL_JoystickClose(joystick);
		joystick = NULL;

		free(axis_neutrals);
	}
#endif
}

const char* ControllerBackend_GetButtonName(unsigned int button_id)
{
	static char name_buffer[0x10];

	snprintf(name_buffer, sizeof(name_buffer), "Button #%d", button_id);

	return name_buffer;
}

#define MIN(a,b) ((a) < (b) ? (a) : (b))
#define MAX(a,b) ((a) > (b) ? (a) : (b))

typedef struct RenderBackend_Surface
{
	SDL_Texture* texture;
	size_t width;
	size_t height;
	bool render_target;
	bool lost;

	struct RenderBackend_Surface* next;
	struct RenderBackend_Surface* prev;
} RenderBackend_Surface;

static RenderBackend_Surface framebuffer;
static RenderBackend_Surface upscaled_framebuffer;
static SDL_Rect window_rect;
static SDL_Renderer* rendererA;

void RenderBackend_HandleWindowResize(size_t width, size_t height)
{
	size_t upscale_factor = MAX(1, MIN((width + framebuffer.width / 2) / framebuffer.width, (height + framebuffer.height / 2) / framebuffer.height));

	upscaled_framebuffer.width = framebuffer.width * upscale_factor;
	upscaled_framebuffer.height = framebuffer.height * upscale_factor;

	if (upscaled_framebuffer.texture != NULL)
	{
		SDL_DestroyTexture(upscaled_framebuffer.texture);
		upscaled_framebuffer.texture = NULL;
	}

	// Create rect that forces 4:3 no matter what size the window is
	if (width * upscaled_framebuffer.height >= upscaled_framebuffer.width * height) // Fancy way to do `if (width / height >= upscaled_framebuffer.width / upscaled_framebuffer.height)` without floats
	{
		window_rect.w = (height * upscaled_framebuffer.width) / upscaled_framebuffer.height;
		window_rect.h = height;
	}
	else
	{
		window_rect.w = width;
		window_rect.h = (width * upscaled_framebuffer.height) / upscaled_framebuffer.width;
	}

	window_rect.x = (width - window_rect.w) / 2;
	window_rect.y = (height - window_rect.h) / 2;

	if (window_rect.w % framebuffer.width != 0 || window_rect.h % framebuffer.height != 0)
	{
		SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");
		upscaled_framebuffer.texture = SDL_CreateTexture(Get_SDL_Renderer(), SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_TARGET, upscaled_framebuffer.width, upscaled_framebuffer.height);
		SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "nearest");

		if (upscaled_framebuffer.texture == NULL)
			Backend_PrintError("Couldn't regenerate upscaled framebuffer");

		SDL_SetTextureBlendMode(upscaled_framebuffer.texture, SDL_BLENDMODE_NONE);
	}
}

static bool keyboard_state[BACKEND_KEYBOARD_TOTAL];

#define DO_KEY(SDL_KEY, BACKEND_KEY) \
	case SDL_KEY: \
		keyboard_state[BACKEND_KEY] = event.key.type == SDL_KEYDOWN; \
		break;

bool Backend_SystemTask(bool active)
{
	if (!active)
		if (!SDL_WaitEvent(NULL))
			return false;

	SDL_Event event;

	while (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
		case SDL_KEYUP:
		case SDL_KEYDOWN:
			switch (event.key.keysym.sym)
			{
					DO_KEY(SDLK_a, BACKEND_KEYBOARD_A)
					DO_KEY(SDLK_b, BACKEND_KEYBOARD_B)
					DO_KEY(SDLK_c, BACKEND_KEYBOARD_C)
					DO_KEY(SDLK_d, BACKEND_KEYBOARD_D)
					DO_KEY(SDLK_e, BACKEND_KEYBOARD_E)
					DO_KEY(SDLK_f, BACKEND_KEYBOARD_F)
					DO_KEY(SDLK_g, BACKEND_KEYBOARD_G)
					DO_KEY(SDLK_h, BACKEND_KEYBOARD_H)
					DO_KEY(SDLK_i, BACKEND_KEYBOARD_I)
					DO_KEY(SDLK_j, BACKEND_KEYBOARD_J)
					DO_KEY(SDLK_k, BACKEND_KEYBOARD_K)
					DO_KEY(SDLK_l, BACKEND_KEYBOARD_L)
					DO_KEY(SDLK_m, BACKEND_KEYBOARD_M)
					DO_KEY(SDLK_n, BACKEND_KEYBOARD_N)
					DO_KEY(SDLK_o, BACKEND_KEYBOARD_O)
					DO_KEY(SDLK_p, BACKEND_KEYBOARD_P)
					DO_KEY(SDLK_q, BACKEND_KEYBOARD_Q)
					DO_KEY(SDLK_r, BACKEND_KEYBOARD_R)
					DO_KEY(SDLK_s, BACKEND_KEYBOARD_S)
					DO_KEY(SDLK_t, BACKEND_KEYBOARD_T)
					DO_KEY(SDLK_u, BACKEND_KEYBOARD_U)
					DO_KEY(SDLK_v, BACKEND_KEYBOARD_V)
					DO_KEY(SDLK_w, BACKEND_KEYBOARD_W)
					DO_KEY(SDLK_x, BACKEND_KEYBOARD_X)
					DO_KEY(SDLK_y, BACKEND_KEYBOARD_Y)
					DO_KEY(SDLK_z, BACKEND_KEYBOARD_Z)
					DO_KEY(SDLK_0, BACKEND_KEYBOARD_0)
					DO_KEY(SDLK_1, BACKEND_KEYBOARD_1)
					DO_KEY(SDLK_2, BACKEND_KEYBOARD_2)
					DO_KEY(SDLK_3, BACKEND_KEYBOARD_3)
					DO_KEY(SDLK_4, BACKEND_KEYBOARD_4)
					DO_KEY(SDLK_5, BACKEND_KEYBOARD_5)
					DO_KEY(SDLK_6, BACKEND_KEYBOARD_6)
					DO_KEY(SDLK_7, BACKEND_KEYBOARD_7)
					DO_KEY(SDLK_8, BACKEND_KEYBOARD_8)
					DO_KEY(SDLK_9, BACKEND_KEYBOARD_9)
					DO_KEY(SDLK_F1, BACKEND_KEYBOARD_F1)
					DO_KEY(SDLK_F2, BACKEND_KEYBOARD_F2)
					DO_KEY(SDLK_F3, BACKEND_KEYBOARD_F3)
					DO_KEY(SDLK_F4, BACKEND_KEYBOARD_F4)
					DO_KEY(SDLK_F5, BACKEND_KEYBOARD_F5)
					DO_KEY(SDLK_F6, BACKEND_KEYBOARD_F6)
					DO_KEY(SDLK_F7, BACKEND_KEYBOARD_F7)
					DO_KEY(SDLK_F8, BACKEND_KEYBOARD_F8)
					DO_KEY(SDLK_F9, BACKEND_KEYBOARD_F9)
					DO_KEY(SDLK_F10, BACKEND_KEYBOARD_F10)
					DO_KEY(SDLK_F11, BACKEND_KEYBOARD_F11)
					DO_KEY(SDLK_F12, BACKEND_KEYBOARD_F12)
					DO_KEY(SDLK_UP, BACKEND_KEYBOARD_UP)
					DO_KEY(SDLK_DOWN, BACKEND_KEYBOARD_DOWN)
					DO_KEY(SDLK_LEFT, BACKEND_KEYBOARD_LEFT)
					DO_KEY(SDLK_RIGHT, BACKEND_KEYBOARD_RIGHT)
					DO_KEY(SDLK_ESCAPE, BACKEND_KEYBOARD_ESCAPE)
					DO_KEY(SDLK_BACKQUOTE, BACKEND_KEYBOARD_BACK_QUOTE)
					DO_KEY(SDLK_TAB, BACKEND_KEYBOARD_TAB)
					DO_KEY(SDLK_CAPSLOCK, BACKEND_KEYBOARD_CAPS_LOCK)
					DO_KEY(SDLK_LSHIFT, BACKEND_KEYBOARD_LEFT_SHIFT)
					DO_KEY(SDLK_LCTRL, BACKEND_KEYBOARD_LEFT_CTRL)
					DO_KEY(SDLK_LALT, BACKEND_KEYBOARD_LEFT_ALT)
					DO_KEY(SDLK_SPACE, BACKEND_KEYBOARD_SPACE)
					DO_KEY(SDLK_RALT, BACKEND_KEYBOARD_RIGHT_ALT)
					DO_KEY(SDLK_RCTRL, BACKEND_KEYBOARD_RIGHT_CTRL)
					DO_KEY(SDLK_RSHIFT, BACKEND_KEYBOARD_RIGHT_SHIFT)
					DO_KEY(SDLK_RETURN, BACKEND_KEYBOARD_ENTER)
					DO_KEY(SDLK_BACKSPACE, BACKEND_KEYBOARD_BACKSPACE)
					DO_KEY(SDLK_MINUS, BACKEND_KEYBOARD_MINUS)
					DO_KEY(SDLK_EQUALS, BACKEND_KEYBOARD_EQUALS)
					DO_KEY(SDLK_LEFTBRACKET, BACKEND_KEYBOARD_LEFT_BRACKET)
					DO_KEY(SDLK_RIGHTBRACKET, BACKEND_KEYBOARD_RIGHT_BRACKET)
					DO_KEY(SDLK_BACKSLASH, BACKEND_KEYBOARD_BACK_SLASH)
					DO_KEY(SDLK_SEMICOLON, BACKEND_KEYBOARD_SEMICOLON)
					DO_KEY(SDLK_QUOTE, BACKEND_KEYBOARD_APOSTROPHE)
					DO_KEY(SDLK_COMMA, BACKEND_KEYBOARD_COMMA)
					DO_KEY(SDLK_PERIOD, BACKEND_KEYBOARD_PERIOD)
					DO_KEY(SDLK_SLASH, BACKEND_KEYBOARD_FORWARD_SLASH)

			default:
				break;
			}

			break;

		case SDL_JOYDEVICEADDED:
			ControllerBackend_JoystickConnect(event.jdevice.which);
			break;

		case SDL_JOYDEVICEREMOVED:
			ControllerBackend_JoystickDisconnect(event.jdevice.which);
			break;

		case SDL_WINDOWEVENT:
			switch (event.window.event)
			{
			case SDL_WINDOWEVENT_FOCUS_LOST:
				csvanilla::InactiveWindow();
				break;
			case SDL_WINDOWEVENT_FOCUS_GAINED:
				csvanilla::ActiveWindow();
				break;

			case SDL_WINDOWEVENT_RESIZED:
			case SDL_WINDOWEVENT_SIZE_CHANGED:
				RenderBackend_HandleWindowResize(event.window.data1, event.window.data2);
				break;
			}

			break;

		case SDL_QUIT:
			csvanilla::StopOrganyaMusic();
			return false;

		case SDL_RENDER_TARGETS_RESET:
			renderer->markRenderTargetsLost();
			break;

		}
	}

	return true;
}


void Backend_GetKeyboardState(bool* out_keyboard_state)
{
	memcpy(out_keyboard_state, keyboard_state, sizeof(keyboard_state));
}