#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "draw.h"
#include "structs.h"
#include "player.h"

void draw_prepare_scene(App * app, SDL_Texture * target)
{
	SDL_SetRenderTarget(app->renderer, target);
	SDL_SetRenderDrawColor(app->renderer, 96, 128, 255, 255);
	SDL_RenderClear(app->renderer);
}

Camera make_camera(App *app, int width, int height) {
    Camera camera = {
        .renderer = app->renderer,
        .x = 0,
        .y = 0,
        .width = width,
        .height = height,
        .target_width = width + CAMERA_BORDER * 2,
        .target_height = height + CAMERA_BORDER * 2,
    };
    camera.target = SDL_CreateTexture(app->renderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_TARGET,
                                      camera.target_width, camera.target_height);
    SDL_assert(camera.target);
    SDL_SetTextureBlendMode(camera.target, SDL_BLENDMODE_BLEND);

    return camera;
}

void draw_camera_to_screen(App *app, Camera *camera) {
        //
        // Draw camera texture
        //
        float pixel_h = (float)SCREEN_HEIGHT / camera->height;
        float correction_x = (int)camera->x - camera->x;
        float correction_y = (int)camera->y - camera->y;

        SDL_Rect dst;
        dst.x = correction_x * pixel_h - pixel_h * CAMERA_BORDER;
        dst.y = correction_y * pixel_h - pixel_h * CAMERA_BORDER;
        dst.w = camera->target_width * pixel_h;
        dst.h = camera->target_height * pixel_h;

        SDL_RenderCopy(app->renderer, camera->target, NULL, &dst);
		SDL_RenderPresent(app->renderer);
}

void camera_update(Camera * camera, struct Entity * player, int map_width, int map_height) {
    // Center camera on map
    camera->x = player->x - (camera->width / 2) + (player->width/2);
    camera->y = player->y - (camera->height / 2) + (player->height/2);
    // Prevent camera from moving outside of map
    if (camera->x < 0) {
        camera->x = 0;
    }
    if (camera->y < 0) {
        camera->y = 0;
    }
    if (camera->x > map_width - camera->width) {
        camera->x = map_width - camera->width;
    }
    if (camera->y > map_height - camera->height) {
        camera->y = map_height - camera->height;
    }
}

void entity_draw(App * app, Tileset * tileset, struct Entity * entity) {
	// Draw entities
    tileset_render_tile(app, tileset, entity->facing, true, entity->x - app->camera->x, entity->y - app->camera->y, 0);
}