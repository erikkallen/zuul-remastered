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
    player->facing = PLAYER_FACING_RIGHT;
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
    // X Axis movement
    if (player->dx < 0) {
        // Move left
        // Check if camera should move or player based on player position
        if (player->x > (camera->width/2) - (player->width/2) || camera->x == 0) {
            player->x += player->dx;
        } else {
            camera->x += player->dx;
        }
    } else {
        // Move right
        if (player->x < (camera->width/2) - (player->width/2) || camera->x == (map->width * map->tilewidth) - camera->width) {
            player->x += player->dx;
        } else {
            camera->x += player->dx;
        }
    }

    // Y Axis movement
    if (player->dy < 0) {
        // Move up
        // Check if camera should move or player based on player position
        if (player->y > (camera->height/2) - (player->height/2) || camera->y == 0) {
            player->y += player->dy;
        } else {
            camera->y += player->dy;
        }
    } else {
        // Move down
        if (player->y < (camera->height/2) - (player->height/2) || camera->y == (map->height * map->tileheight) - camera->height) {
            player->y += player->dy;
        } else {
            camera->y += player->dy;
        }
    }

    // Debug camera position
    // log_debug("Camera x: %f y: %f", camera->x, camera->y);
    // Debug player position
    // log_debug("Player x: %d y: %d", player->x, player->y);


    // Prevent camera from moving outside of map
    if (camera->x < 0) {
        camera->x = 0;
    }
    if (camera->y < 0) {
        camera->y = 0;
    }
    if (camera->x > (map->width * map->tilewidth) - camera->width) {
        camera->x = (map->width * map->tilewidth) - camera->width;
    }
    if (camera->y > (map->height * map->tileheight) - camera->height) {
        camera->y = (map->height * map->tileheight) - camera->height;
    }

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

    // Reset player dx and dy
    player->dx = 0;
    player->dy = 0;
}

void player_draw(App * app, struct Entity *entity)
{
	// Draw player
    tileset_render_tile(app, entity->tileset, entity->facing, entity->x, entity->y, entity->move_speed > 0 ? 1 : 0);
}

void player_free(struct Entity * player) {
    // Free any memory allocated for the player
    // Free any memory allocated for the player's texture
    // Free any memory allocated for the player's animation
    // Free the player
    
}