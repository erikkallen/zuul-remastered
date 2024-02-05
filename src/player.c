#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "structs.h"
#include "draw.h"
#include "tileset.h"
#include "player.h"

// Logging
#include <log.h>

#define PLAYER_BORDER_DISTANCE 10
int player_init(App * app, struct Entity * player, Tileset * tileset) {
    player->x = 100;
    player->y = 100;
    player->width = tileset->tile_width;
    player->height = tileset->tile_height;
    player->tileset = tileset;
    return 0;
}

void player_handle(App * app, Map * map, Camera *camera, struct Entity * player) {
    if (app->keyboard[SDL_SCANCODE_UP]) {
        player->dy -= PLAYER_SPEED;
        //camera->y -= PLAYER_SPEED;
        player->facing = PLAYER_FACING_UP;
    }
    if (app->keyboard[SDL_SCANCODE_DOWN]) {
        player->dy += PLAYER_SPEED;
        //camera->y += PLAYER_SPEED;
        player->facing = PLAYER_FACING_DOWN;
    }
    if (app->keyboard[SDL_SCANCODE_LEFT]) {
        player->dx -= PLAYER_SPEED;
        //camera->x -= PLAYER_SPEED;
        player->facing = PLAYER_FACING_LEFT;
    }
    if (app->keyboard[SDL_SCANCODE_RIGHT]) {
        player->dx += PLAYER_SPEED;
        //camera->x += PLAYER_SPEED;
        player->facing = PLAYER_FACING_RIGHT;
    }
    if (app->keyboard[SDL_SCANCODE_RIGHT] || app->keyboard[SDL_SCANCODE_LEFT] || app->keyboard[SDL_SCANCODE_UP] || app->keyboard[SDL_SCANCODE_DOWN]) {
        player->move_speed = PLAYER_SPEED;
    } else{
        player->move_speed = 0;
    }
    // Keep player in the center of the camera until the camera hits the edge of the map
    // camera->x += player->dx;
    // camera->y += player->dy;
    if (camera->x <= 0) {
        camera->x = 0;
        if (player->dx < 0) {
            // Move left
            player->x += player->dx;
        } else {
            // Move right
            if (player->x <= (camera->width/2) - (player->width/2)) {
                player->x += player->dx;
            } else {
                camera->x += player->dx;
            }
        }
    } else if (camera->x >= (map->width * map->tile_width) - camera->width) {
        camera->x = (map->width * map->tile_width) - camera->width;
        if (player->dx > (camera->width/2) - (player->width/2)) {
            // Move right
            player->x += player->dx;
        } else {
            // Move left
            if (player->x >= (camera->width/2) - (player->width/2)) {
                player->x += player->dx;
            } else {
                camera->x += player->dx;
            }
        }
    } else {
        camera->x += player->dx;
    }
    if (camera->y <= 0) {
        camera->y = 0;
        if (player->dy < 0) {
            // Move up
            player->y += player->dy;
        } else {
            // Move down
            if (player->y <= (camera->height/2) - (player->height/2)) {
                player->y += player->dy;
            } else {
                camera->y += player->dy;
            }
        }
    } else if (camera->y >= (map->height * map->tile_height) - camera->height) {
        camera->y = (map->height * map->tile_height) - camera->height;
        if (player->dy > (camera->height/2) - (player->height/2)) {
            // Move down
            player->y += player->dy;
        } else {
            // Move up
            if (player->y >= (camera->height/2) - (player->height/2)) {
                player->y += player->dy;
            } else {
                camera->y += player->dy;
            }
        }
    } else {
        camera->y += player->dy;
    }
    // Debug camera position
    log_debug("Camera x: %f y: %f", camera->x, camera->y);
    // Debug player position
    log_debug("Player x: %d y: %d", player->x, player->y);



    // Prevent player from moving outside of map
    if (player->x < 0) {
        player->x = 0;
    }
    if (player->y < 0) {
        player->y = 0;
    }
    if (player->x > camera->width - player->width) {
        player->x = camera->width - player->width;
    }
    if (player->y > camera->height - player->height) {
        player->y = camera->height - player->height;
    }

    // player->x += player->dx;
    // player->y += player->dy;
    // Debug player position
    log_debug("Player x: %d y: %d", player->x, player->y);
    // camera->x = camera->width;

    // Move camera if player gets close to edge but stop if camera hits map border


    // Reset player dx and dy
    player->dx = 0;
    player->dy = 0;
}

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