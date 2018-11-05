#ifdef USE_SDL2
#include <SDL.h>

static SDL_Window* window = NULL;
static SDL_Texture* texture = NULL;
static SDL_Renderer* renderer = NULL;
static int x_offset;
static int y_offset;
static int scaled_height;
static int scaled_width;
static int display_width;
static int display_height;
extern int mainMenu_shader;
extern int mainMenu_displayedLines;
extern int mainMenu_displayHires;
extern int visibleAreaWidth;
extern int displaying_menu;
//uint32_t *texture_pixels=NULL;

#define MIN(a,b) ((a) < (b) ? (a) : (b))

SDL_Surface *SDL_SetVideoMode(int w, int h, int bpp, int flags) {
	if (!renderer) {
		window = SDL_CreateWindow("uae4all2", 0, 0, 0, 0, SDL_WINDOW_FULLSCREEN_DESKTOP);
		renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
		SDL_RenderClear(renderer);
		SDL_DisplayMode DM;
		SDL_GetCurrentDisplayMode(0, &DM);
		display_width = DM.w;
		display_height = DM.h;
	}
	SDL_Surface *surface = SDL_CreateRGBSurfaceWithFormat(0, w, h, 16, SDL_PIXELFORMAT_RGB565);
	//if (texture) {
	//	SDL_DestroyTexture(texture);
	//	texture = NULL;
	//}
	//texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, w, h);
	return surface;
}

void SDL_SetVideoModeScaling(int x, int y, float sw, float sh) {
	if (mainMenu_shader != 0 && !displaying_menu) {
		x_offset = (x * display_width) / 960;
		y_offset = (y * display_height) / 544;
		scaled_width = (sw * display_width) / 960;
		scaled_height = (sh * display_height) / 544;
	} else {
		int screen_width = 320;
		int screen_height = 240;
		if (!displaying_menu) {
			screen_width = visibleAreaWidth;
			if (mainMenu_displayHires)
				screen_height = 2 * mainMenu_displayedLines;			
			else
				screen_height = mainMenu_displayedLines;
		}
		int scale_factor = MIN(display_height / screen_height, display_width / screen_width);
		scaled_height = (float) (scale_factor * screen_height);
		scaled_width = (float) (scale_factor * screen_width);
		x_offset = (display_width - scaled_width) / 2;
		y_offset = (display_height - scaled_height) / 2;
	}
}

void SDL_SetVideoModeBilinear(int value) {
	if (value)
		SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");
	else
		SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "nearest");
}

void SDL_SetVideoModeSync(int value) {
	if (1)
		SDL_SetHint(SDL_HINT_RENDER_VSYNC, "1");
	else
		SDL_SetHint(SDL_HINT_RENDER_VSYNC, "0");
}

void SDL_Flip(SDL_Surface *surface) {
	if (surface && renderer && window) {
		//SDL_UpdateTexture(texture, NULL, surface->pixels, surface->pitch);
		Uint32 *texture_pixels = NULL;
		int texture_pitch;
		Uint32 texture_format;
		//SDL_QueryTexture(texture, &texture_format, NULL, NULL, NULL);
		//SDL_LockTexture(texture, NULL, (void **) texture_pixels, &texture_pitch);
		//SDL_ConvertPixels(surface->w, surface->h, surface->format->format, surface->pixels, surface->pitch, texture_format, texture_pixels, texture_pitch);
		//SDL_UnlockTexture(texture);
		if (texture) {
			SDL_DestroyTexture(texture);
			texture = NULL;
		}
		texture = SDL_CreateTextureFromSurface(renderer, surface);

		SDL_Rect dst_rect = { x_offset, y_offset, scaled_width, scaled_height };
		SDL_Rect *src_rect = NULL;
		SDL_RenderCopy(renderer, texture, src_rect, &dst_rect);
		SDL_RenderPresent(renderer);
	}
}
#endif