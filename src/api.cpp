#include "api.h"
#include "rendering.h"
#include <SDL.h>

// Global variables for storing mouse state and coordinates
static Uint32 mouseButtons = 0;
static int mouseX = 0, mouseY = 0;

extern "C" {
	SDL_Window* Get_SDL_Window()
	{
		if (renderer != nullptr)
			return renderer->window;
		else
			return nullptr;
	}
	SDL_Renderer* Get_SDL_Renderer()
	{
		if (renderer != nullptr)
			return renderer->renderer;
		else
			return nullptr;
	}
	void* Get_Surface(int surfaceID)
	{
		if (renderer != nullptr)
		{
			if (surfaceID == RenderBackend::FramebufferID)
				return &renderer->framebuffer;
			else if (surfaceID >= 0 && surfaceID < RenderBackend::NumSurfaces)
				return &renderer->surf[surfaceID];
		}
		return nullptr;
	}
}

Uint32 Get_SDL_MouseState(int* x, int* y)
{
	// Update the global mouse state and coordinates
	mouseButtons = SDL_GetMouseState(&mouseX, &mouseY);

	if (x != nullptr)
		*x = mouseX;
	if (y != nullptr)
		*y = mouseY;

	return mouseButtons;
}

const Uint8* Get_SDL_KeyboardState()
{
	// Return the current state of the keyboard keys
	return SDL_GetKeyboardState(NULL);
}

void PutBitmap3Alpha(const RECT* rcView, int x, int y, const RECT* rect, int surf_no, Uint8 alpha, double angle, Uint32 color)
{
	PutBitmap3A(rcView, x, y, rect, surf_no, alpha, angle, color);
}

void PutMax2x2Api(const RECT* src, float x0, float y0, float x1, float y1, float x2, float y2, float x3, float y3, int surf_no, Uint32 color, Uint8 alpha)
{
	PutMat2x2(src, x0, y0, x1, y1, x2, y2, x3, y3, surf_no, color, alpha);
}