#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "draw.h"

void prepareScene(App * app)
{
	SDL_SetRenderDrawColor(app->renderer, 96, 128, 255, 255);
	SDL_RenderClear(app->renderer);
}

void presentScene(App * app)
{
	SDL_RenderPresent(app->renderer);
}

SDL_Texture *loadTexture(App * app, char *filename)
{
	SDL_Texture *texture;

	SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO, "Loading %s", filename);

	texture = IMG_LoadTexture(app->renderer, filename);

	return texture;
}