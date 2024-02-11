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
    player->x = 101;
    player->y = 101;
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
    
    int player_x = player->x + camera->x + player->dx;
    int player_y = player->y + camera->y + player->dy;

    // Get player tile position
    int player_tile_x = player_x;
    int player_tile_y = player_y;

    int bounding_box_x = player_tile_x;
    int bounding_box_y = player_tile_y;
    int bounding_box_width = player->width;
    int bounding_box_height = player->height;
    // Get player tile_id from tilemap and find bounding box
    Tile * player_tile = tileset_get_tile_by_id(player->tileset, player->facing, true);
    if (player_tile == NULL) {
        log_error("Player tile not found");
        return;
    }

    // Get bounding box position
    for (int i=0;i<player_tile->objectgroup_count;i++) {
        Layer * objectgroup = player_tile->objectgroup;
        // log_debug("Object x: %d y: %d width: %d height: %d type: %s", objectgroup[i].x, objectgroup[i].y, objectgroup[i].width, objectgroup[i].height, objectgroup[i].type);
        if (strcmp(objectgroup[i].type, "collision_box") == 0) {
            bounding_box_x = (player_x + objectgroup[i].x);
            bounding_box_y = (player_y + objectgroup[i].y);
            bounding_box_width = objectgroup[i].width;
            bounding_box_height = objectgroup[i].height;
            break;
        }
    }
    

    SDL_Rect player_rect = {bounding_box_x, bounding_box_y, bounding_box_width, bounding_box_height};
    // Check for collision for each_tile under the player
    int player_tile_col = bounding_box_x / map->tilewidth;
    int player_tile_row = bounding_box_y / map->tileheight;
    int player_tile_col_end = (bounding_box_x + bounding_box_width) / map->tilewidth;
    int player_tile_row_end = (bounding_box_y + bounding_box_height) / map->tileheight;
    for (int i=player_tile_col;i <= player_tile_col_end + 1; i++) {
        for (int j=player_tile_row;j <= player_tile_row_end + 1; j++) {
            //log_debug("Checking collision at x: %d y: %d", i, j);
            if (map_check_collision(map, i, j, &player_rect)) {
                log_debug("Collision detected at x: %d [%d %d] y: %d [%d %d]", i,player_tile_col, player_tile_col_end, j, player_tile_row, player_tile_row_end);
                // Prevent player from moving
                //if (i == player_tile_col || i == (player_tile_col_end + 1)) {
                    player->dx = 0;
                //}
                //if (j == player_tile_row || j == (player_tile_row_end + 1)) {
                    player->dy = 0;
                //}
                // player->dx = 0;
                // player->dy = 0;
                break;
            }
        }
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
    // int player_x = entity->x + app->camera->x + entity->dx;
    // int player_y = entity->y + app->camera->y + entity->dy;

    // SDL_Rect player_rect = {player_x, player_y, entity->width, entity->height};
    // SDL_SetRenderDrawColor(app->renderer,0,0,255,255);
    // SDL_RenderFillRect(app->renderer,
    //                    &player_rect);
    // SDL_SetRenderDrawColor(app->renderer,255,0,0,255);
    // SDL_RenderDrawPoint(app->renderer,player_x,player_y);
    // Get player bounding box as SDL_Rect
    tileset_render_tile(app, entity->tileset, entity->facing, true, entity->x, entity->y, entity->move_speed > 0 ? 1 : 0);
}

void player_free(struct Entity * player) {
    // Free any memory allocated for the player
    // Free any memory allocated for the player's texture
    // Free any memory allocated for the player's animation
    // Free the player
    
}