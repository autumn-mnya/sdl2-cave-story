#pragma once

#include <cstddef>
#include "api.h"

typedef struct tagRECT RECT;
class Bitmap;
struct RenderBackend_GlyphAtlas;
class RenderBackend
{
	static constexpr int NumSurfaces = 800;

	// Based on the CSE2-portable SDLTexture backend
	struct Surface
	{
		struct SDL_Texture* texture;
		unsigned width;
		unsigned height;
		bool render_target;
		bool lost;
	};

	struct SDL_Window* window;
	struct SDL_Renderer* renderer;
	Surface framebuffer;
	Surface surf[NumSurfaces];

	RenderBackend_GlyphAtlas* glyph_atlas;

	Surface& getSurface(int id);

	void prepareToDrawGlyphs(RenderBackend_GlyphAtlas* atlas, int surf_no, unsigned char red, unsigned char green, unsigned char blue);
	void drawGlyph(long x, long y, std::size_t glyph_x, std::size_t glyph_y, std::size_t glyph_width, std::size_t glyph_height);

	friend SDL_Window* Get_SDL_Window();
	friend SDL_Renderer* Get_SDL_Renderer();
	friend void* Get_Surface(int surfaceID);
	friend void* GetSurf();
	friend void PutBitmap3A(const RECT* rcView, int x, int y, const RECT* rect, int surf_no, Uint8 alpha, double angle, Uint32 color);
	friend void PutMat2x2(const RECT* src, float x0, float y0, float x1, float y1, float x2, float y2, float x3, float y3, int surf_no, Uint32 color, Uint8 alpha);
public:
	RenderBackend();
	~RenderBackend();

	RenderBackend(const RenderBackend&) = delete;
	RenderBackend(RenderBackend&&) = delete;
	RenderBackend& operator=(const RenderBackend&) = delete;
	RenderBackend& operator=(RenderBackend&&) = delete;

	bool initWindow(int width, int height, int magnification);
	void deinit();

	void drawScreen();
	bool createSurface(int surf_no, int width, int height, bool isRenderTarget);
	bool copyBitmapOntoSurface(int surf_no, const Bitmap& bitmap);
	void releaseSurface(int surf_no);
	void markRenderTargetsLost();
	int restoreSurfaces();

	static constexpr int FramebufferID = -1;
	void blit(int srcSurfaceID, const RECT& rect, int destSurfaceID, int x, int y, bool useColorKey);
	void colorFill(int surfaceID, const RECT& rect, unsigned long color);

	RenderBackend_GlyphAtlas* createGlyphAtlas(std::size_t width, std::size_t height);
	void drawText(struct Font* font, int surf_no, int x, int y, unsigned long color, const char* string);
};

extern RenderBackend* renderer;
extern void PutBitmap3A(const RECT* rcView, int x, int y, const RECT* rect, int surf_no, Uint8 alpha, double angle, Uint32 color);
extern void PutMat2x2(const RECT* src, float x0, float y0, float x1, float y1, float x2, float y2, float x3, float y3, int surf_no, Uint32 color, Uint8 alpha);

typedef void (*SdlStartDirectDrawElementHandler)();
extern "C" __declspec(dllexport) void RegisterSDLStartDirectDrawElement(SdlStartDirectDrawElementHandler handler);

void BackupSurface(int surf_no, const RECT* rect);
void PutBitmap3(const RECT* rcView, int x, int y, const RECT* rect, int surf_no);
void PutBitmap4(const RECT* rcView, int x, int y, const RECT* rect, int surf_no);
void Surface2Surface(int x, int y, const RECT* rect, int to, int from);
unsigned long GetCortBoxColor(unsigned long col);
void CortBox(const RECT* rect, unsigned long col);
void CortBox2(const RECT* rect, unsigned long col, int surf_no);
int RestoreSurfaces();
