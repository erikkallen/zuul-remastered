#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "draw.h"
#include "structs.h"
#include "player.h"

void draw_prepare_scene(App * app)
{
	SDL_SetRenderDrawColor(app->renderer, 96, 128, 255, 255);
	SDL_RenderClear(app->renderer);
}

void draw_present_scene(App * app)
{
	SDL_RenderPresent(app->renderer);
}

int draw_load_texture(App * app, struct TextureImage * texture)
{
	SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO, "Loading %s", texture->filename);

	texture->texture = IMG_LoadTexture(app->renderer, texture->filename);
        
	if (texture->texture == NULL)
	{
		SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO, "IMG_LoadTexture Error: %s\n", SDL_GetError());
		return 1;
	}

	return 0;
}

void draw_blit_texture(App * app, struct Entity *entity)
{
	SDL_Rect dest;
	SDL_Rect *src = player_get_current_frame(entity);

	dest.x = entity->x;
	dest.y = entity->y;
	dest.w = src->w;
	dest.h = src->h;

	SDL_RenderCopy(app->renderer, entity->texture.texture, src, &dest);
}