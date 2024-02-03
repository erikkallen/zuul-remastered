#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "structs.h"
#include "draw.h"
#include "tileset.h"
#include "player.h"

// Logging
#include <log.h>

#define SPRITE_NUM_FRAMES 8
#define ANIMATION_FRAMES 2
int player_init(App * app, struct Entity * player, Tileset * tileset) {
    player->x = 100;
    player->y = 100;
    player->width = tileset->tile_width;
    player->height = tileset->tile_height;
    player->tileset = tileset;
    return 0;
}

void player_handle(App * app, struct Entity * player) {
    if (app->keyboard[SDL_SCANCODE_UP]) {
        player->y -= PLAYER_SPEED;
        player->facing = PLAYER_FACING_UP;
    }
    if (app->keyboard[SDL_SCANCODE_DOWN]) {
        player->y += PLAYER_SPEED;
        player->facing = PLAYER_FACING_DOWN;
    }
    if (app->keyboard[SDL_SCANCODE_LEFT]) {
        player->x -= PLAYER_SPEED;
        player->facing = PLAYER_FACING_LEFT;
    }
    if (app->keyboard[SDL_SCANCODE_RIGHT]) {
        player->x += PLAYER_SPEED;
        player->facing = PLAYER_FACING_RIGHT;
    }
    if (app->keyboard[SDL_SCANCODE_RIGHT] || app->keyboard[SDL_SCANCODE_LEFT] || app->keyboard[SDL_SCANCODE_UP] || app->keyboard[SDL_SCANCODE_DOWN]) {
        player->move_speed = PLAYER_SPEED;
    } else{
        player->move_speed = 0;
    }
    // player->x += player->dx;
    // player->y += player->dy;
}

// SDL_Rect * player_get_current_frame(struct Entity * player) {
//     SDL_Rect * frame = &player->texture.frames[player->texture.current_frame];
//     int ticks = SDL_GetTicks();
//     int sprite = (ticks / player->texture.animation_speed) % ANIMATION_FRAMES;
//     if (player->move_speed == 0) {
//         sprite = 0;
//     }
//     player->texture.current_frame = player->texture.frame_offset + sprite;//(player->texture.current_frame + 1) % SPRITE_NUM_FRAMES;
//     return frame;
// }

void player_draw(App * app, struct Entity *entity)
{
	SDL_Rect dest;
    Frame * frame;
	Frame *frames = entity->tileset->tile_image.frames;
    SDL_Texture *texture = entity->tileset->tile_image.texture;

    switch (entity->facing) {
        case PLAYER_FACING_UP:
            frame = &frames[4];
            break;
        case PLAYER_FACING_DOWN:
            frame = &frames[6];
            break;
        case PLAYER_FACING_LEFT:
            frame = &frames[0];
            break;
        case PLAYER_FACING_RIGHT:
            frame = &frames[2];
            break;
    }

    SDL_Rect * src = &frame->frame;
    if (frame->animation != NULL && entity->move_speed > 0) {
        src = &frames[tileset_get_animation_frame_id(frame)].frame;
    }
    
    // Get texture from tileset

	dest.x = entity->x;
	dest.y = entity->y;
	dest.w = src->w;
	dest.h = src->h;

	SDL_RenderCopy(app->renderer, texture, src, &dest);
}

void player_free(struct Entity * player) {
    // Free any memory allocated for the player
    // Free any memory allocated for the player's texture
    // Free any memory allocated for the player's animation
    // Free the player
    
}