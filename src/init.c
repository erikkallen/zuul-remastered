#include <SDL2/SDL.h>
#include <stdio.h>
#include "structs.h"

// Logging
#include <log.h>

void init_sdl(App * app)
{
	int rendererFlags, windowFlags;

	rendererFlags = SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC;

	windowFlags = 0;

	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) < 0)
	{
		log_error("Couldn't initialize SDL: %s\n", SDL_GetError());
		exit(1);
	}

	if (IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG) < 0)
	{
		log_error("Couldn't initialize IMG: %s\n", SDL_GetError());
		exit(1);
	}

	app->window = SDL_CreateWindow("Shooter 01", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, windowFlags);
	SDL_assert(app->window);

	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");
	SDL_SetHint(SDL_HINT_RENDER_BATCHING, "1");
	app->renderer = SDL_CreateRenderer(app->window, -1, rendererFlags);
	SDL_assert(app->renderer);
}