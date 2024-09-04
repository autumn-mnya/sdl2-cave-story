#pragma once

#include <SDL.h>

typedef struct tagRECT RECT;

// Functions to be exported from the DLL so that other programs can access the SDL rendering facilities
// that this DLL mod enables

struct SDL_Window;
struct SDL_Renderer;

extern "C" {
	// Retrieves the SDL_Window object for this instance. Returns null if the rendering backend hasn't been initialized yet.
	SDL_Window* Get_SDL_Window();

	// Retrieves the SDL_Renderer object for this instance. Returns null if the rendering backend hasn't been initialized yet.
	SDL_Renderer* Get_SDL_Renderer();

	// Retrieves a pointer to the internal RenderBackend::Surface struct for the given surface ID.
	// See rendering.h for the definition of this struct. To use this function in your own code, declare an identical Surface struct
	// and then cast the return value of this function from void* to Surface*.
	// Returns null if the rendering backend hasn't been initialized yet, or if the specified surfaceID is out of bounds.
	// The surface IDs are mapped in the same way as vanilla CS. You can also pass -1 to retrieve the main framebuffer object.
	void* Get_Surface(int surfaceID);
}

extern "C" __declspec(dllexport) void PutBitmap3Alpha(const RECT* rcView, int x, int y, const RECT* rect, int surf_no, Uint8 alpha);
extern "C" __declspec(dllexport) Uint32 Get_SDL_MouseState(int* x, int* y);
extern "C" __declspec(dllexport) const Uint8* Get_SDL_KeyboardState();