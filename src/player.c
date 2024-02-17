#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "structs.h"
#include "draw.h"
#include "tileset.h"
#include "player.h"
#include <math.h>

#include "assets.h"

// Logging
#include <log.h>

struct Entity player;
Camera * cam;

#define PLAYER_BORDER_DISTANCE 10
int player_init(App * app, Tileset * tileset) {
    player.x = 101;
    player.y = 101;
    player.width = tileset->tile_width;
    player.height = tileset->tile_height;
    player.tileset = tileset;
    player.facing = PLAYER_FACING_RIGHT;
    // Ugly hack to get the player to start in the middle of the screen
    cam = app->camera;
    return 0;
}

void camera_bounds_check(Camera * camera, Map * map) {
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
}

void player_reset_camera(Camera * camera, Map * map, int to_x, int to_y) {
    cam->x = to_x - (cam->width/2);
    cam->y = to_y - (cam->height/2);
    camera_bounds_check(cam, map);
    player.x = to_x - cam->x;
    // Dont know why we need to subtract the player height
    player.y = to_y - cam->y - (player.height);
}

void collision_callback(Property *property, void *data) {
    log_debug("Collision detected with object: %s", property->string_value);
    // Load new map
    Map * map = (Map *)data;
    // Save tileset and property value before beeing freed
    Tileset * tileset = map->tileset;
    int to_x, to_y;
    char asset_name[99];
    // Get x an y from property
    // Use sscanf to parse the string
    if (sscanf(property->string_value, "%99[^:]:%d,%d", asset_name, &to_x, &to_y) == 3) {
        // Successfully parsed
        log_debug("Warping to map: %s x: %d y: %d", asset_name, to_x, to_y);

        char * map_path = asset_path(asset_name);
        // log_debug("Map width: %d height: %d", map->width, map->height);
        map_free(map);
        map_init(map, tileset, map_path);
        // log_debug("Map width: %d height: %d", map->width, map->height);
        // log_debug("Player x: %d y: %d", player.x, player.y);

        player_reset_camera(cam, map, to_x, to_y);
        // log_debug("Player x: %d y: %d dx: %f dy: %f",player.x, player.y,  player.dx, player.dy);
    } else {
        // Parsing failed
        log_error("Failed to parse the input string: %s\nShould be in the format filename.tmj:x,y where x and y are coordinate to span the player on the new map", property->string_value);
    }
}

void player_handle(App * app, Map * map, Camera *camera) {
    if (app->keyboard[SDL_SCANCODE_UP]) {
        player.dy -= PLAYER_SPEED;
        //camera->y -= PLAYER_SPEED;
        player.facing = PLAYER_FACING_UP;
    }
    if (app->keyboard[SDL_SCANCODE_DOWN]) {
        player.dy += PLAYER_SPEED;
        //camera->y += PLAYER_SPEED;
        player.facing = PLAYER_FACING_DOWN;
    }
    if (app->keyboard[SDL_SCANCODE_LEFT]) {
        player.dx -= PLAYER_SPEED;
        //camera->x -= PLAYER_SPEED;
        player.facing = PLAYER_FACING_LEFT;
    }
    if (app->keyboard[SDL_SCANCODE_RIGHT]) {
        player.dx += PLAYER_SPEED;
        //camera->x += PLAYER_SPEED;
        player.facing = PLAYER_FACING_RIGHT;
    }
    if (app->keyboard[SDL_SCANCODE_RIGHT] || app->keyboard[SDL_SCANCODE_LEFT] || app->keyboard[SDL_SCANCODE_UP] || app->keyboard[SDL_SCANCODE_DOWN]) {
        player.move_speed = PLAYER_SPEED;
    } else{
        player.move_speed = 0;
    }
    
    player_move_to(camera, map, player.x, player.y);
    // log_debug("Player x: %d y: %d abs %d %d",player.x, player.y, player.x_abs, player.y_abs);
}

SDL_Rect player_get_collision_rect(int x, int y) {
    SDL_Rect rect = {x, y, player.width, player.height};
    // Get player tile_id from tilemap and find bounding box
    Tile * player_tile = tileset_get_tile_by_id(player.tileset, player.facing, true);
    if (player_tile == NULL) {
        log_error("Player tile not found");
        return rect;
    }
    int player_x = x;
    int player_y = y;
    // Get bounding box position
    for (int i=0;i<player_tile->objectgroup_count;i++) {
        Layer * objectgroup = player_tile->objectgroup;
        // log_debug("Object x: %d y: %d width: %d height: %d type: %s", objectgroup[i].x, objectgroup[i].y, objectgroup[i].width, objectgroup[i].height, objectgroup[i].type);
        if (strcmp(objectgroup[i].type, "collision_box") == 0) {
            int bounding_box_x = (player_x + objectgroup[i].x);
            int bounding_box_y = (player_y + objectgroup[i].y);
            int bounding_box_width = objectgroup[i].width;
            int bounding_box_height = objectgroup[i].height;
            rect.x = bounding_box_x;
            rect.y = bounding_box_y;
            rect.w = bounding_box_width;
            rect.h = bounding_box_height;
            return rect;
        }
    }

    // If not bounding box was found return player tile position
    return rect;
}

bool player_check_map_collision(Map * map, SDL_Rect * player_rect) {
    // Check for collision for each_tile under the player
    int player_tile_col = player_rect->x / map->tilewidth;
    int player_tile_row = player_rect->y / map->tileheight;
    int player_tile_col_end = (player_rect->x + player_rect->w) / map->tilewidth;
    int player_tile_row_end = (player_rect->y + player_rect->h) / map->tileheight;
    for (int i=player_tile_col;i <= player_tile_col_end; i++) {
        for (int j=player_tile_row;j <= player_tile_row_end; j++) {
            //log_debug("Checking collision at x: %d y: %d", i, j);
            if (map_check_tile_collision(map, i, j, player_rect)) {
                log_debug("Collision detected at x: %d [%d %d] y: %d [%d %d]", i,player_tile_col, player_tile_col_end, j, player_tile_row, player_tile_row_end);
                // Prevent player from moving
                // Fix player direction during collision
                return true;
            }
        }
    }
    return false;
}

void player_move_to(Camera * camera, Map * map, int x, int y) {
    player.x = x;
    player.y = y;

    // Check collision in x direction
    SDL_Rect player_rect = player_get_collision_rect(player.x_abs + player.dx, player.y_abs);
    if (player_check_map_collision(map, &player_rect)) {
        // Fix player direction during collision
        if (player.dy < 0) {
            // Move left
            player.facing = PLAYER_FACING_UP;
        } else if (player.dy > 0){
            // Move right
            player.facing = PLAYER_FACING_DOWN;
        }
        player.dx = 0;
    }
    // Check collision in y direction
    player_rect = player_get_collision_rect(player.x_abs, player.y_abs + player.dy);
    if (player_check_map_collision(map, &player_rect)) {
        // Fix player direction during collision
        if (player.dx < 0) {
            // Move up
            player.facing = PLAYER_FACING_LEFT;
        } else if (player.dx > 0){
            // Move down
            player.facing = PLAYER_FACING_RIGHT;
        }
        player.dy = 0;
    }
    // Map object collision check
    map_check_object_collisions(map, "warp", &player_rect, collision_callback);

    // Keep player in the center of the camera until the camera hits the edge of the map
    // X Axis movement
    if (player.dx < 0) {
        // Move left
        // Check if camera should move or player based on player position
        if (player.x > (camera->width/2) - (player.width/2) || camera->x == 0) {
            player.x += player.dx;
        } else {
            camera->x += player.dx;
        }
    } else {
        // Move right
        if (player.x < (camera->width/2) - (player.width/2) || camera->x == (map->width * map->tilewidth) - camera->width) {
            player.x += player.dx;
        } else {
            camera->x += player.dx;
        }
    }

    // Y Axis movement
    if (player.dy < 0) {
        // Move up
        // Check if camera should move or player based on player position
        if (player.y > (camera->height/2) - (player.height/2) || camera->y == 0) {
            player.y += player.dy;
        } else {
            camera->y += player.dy;
        }
    } else {
        // Move down
        if (player.y < (camera->height/2) - (player.height/2) || camera->y == (map->height * map->tileheight) - camera->height) {
            player.y += player.dy;
        } else {
            camera->y += player.dy;
        }
    }

    // Debug camera position
    // log_debug("Camera x: %f y: %f", camera->x, camera->y);
    // Debug player position
    // log_debug("Player x: %d y: %d", player.x, player.y);


    // Prevent camera from moving outside of map
    camera_bounds_check(camera, map);

    // Prevent player from moving outside of map
    if (player.x < 0) {
        player.x = 0;
    }
    if (player.y < 0) {
        player.y = 0;
    }
    if (player.x > camera->width - player.width) {
        player.x = camera->width - player.width;
    }
    if (player.y > camera->height - player.height) {
        player.y = camera->height - player.height;
    }
    // Reset player dx and dy
    player.dx = 0;
    player.dy = 0;
    // Update player absolute position
    player.x_abs = player.x + camera->x;
    player.y_abs = player.y + camera->y;
}

void player_draw(App * app)
{
	// Draw player
    tileset_render_tile(app, player.tileset, player.facing, true, player.x, player.y, player.move_speed > 0 ? 1 : 0);
}

void player_free() {
    // Free any memory allocated for the player
    // Free any memory allocated for the player's texture
    // Free any memory allocated for the player's animation
    // Free the player
    
}