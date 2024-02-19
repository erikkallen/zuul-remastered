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

#define PLAYER_BORDER_DISTANCE 10


int player_init(App * app, Tileset * tileset) {
    player.x = 160;
    player.y = 187;
    
    player.width = tileset->tile_width;
    player.height = tileset->tile_height;
    player.tileset = tileset;
    player.facing = PLAYER_FACING_DOWN;
    // Ugly hack to get the player to start in the middle of the screen
    return 0;
}

struct Entity * player_get() {
    return &player;
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

        player.x = to_x;
        player.y = to_y;
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
    
    player_move(map);
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

bool player_check_map_collision(Map * map, SDL_Rect * player_rect, SDL_Rect * intersection) {
    // Check for collision for each_tile under the player
    int player_tile_col = player_rect->x / map->tilewidth;
    int player_tile_row = player_rect->y / map->tileheight;
    int player_tile_col_end = (player_rect->x + player_rect->w) / map->tilewidth;
    int player_tile_row_end = (player_rect->y + player_rect->h) / map->tileheight;
    for (int i=player_tile_col;i <= player_tile_col_end; i++) {
        for (int j=player_tile_row;j <= player_tile_row_end; j++) {
            //log_debug("Checking collision at x: %d y: %d", i, j);
            if (map_check_tile_collision(map, i, j, player_rect, intersection)) {
                //log_debug("Collision box x: %d y: %d w: %d h: %d", intersection->x, intersection->y, intersection->w, intersection->h);
                // Prevent player from moving
                // Fix player direction during collision
                //log_debug("Collision detected at x: %d [%d %d] y: %d [%d %d]", i,player_tile_col, player_tile_col_end, j, player_tile_row, player_tile_row_end);
                return true;
            }
        }
    }
    return false;
}

void player_move(Map * map) {
    int map_width = map->width * map->tilewidth;
    int map_height = map->height * map->tileheight;
    // Check collision in x direction
    SDL_Rect player_rect = player_get_collision_rect(player.x + player.dx, player.y);
    SDL_Rect distance = {0, 0, 0, 0};
    if (player_check_map_collision(map, &player_rect, &distance)) {
        // Fix player direction during collision
        if (player.dy < 0) {
            // Move left
            player.facing = PLAYER_FACING_UP;
        } else if (player.dy > 0){
            // Move right
            player.facing = PLAYER_FACING_DOWN;
        }
        log_debug("Collision detected at x: %d y: %d w: %d h: %d", player.x, player.y, distance.w, distance.h);
        player.dx = PLAYER_SPEED - distance.w;
    }
    // Check collision in y direction
    player_rect = player_get_collision_rect(player.x, player.y + player.dy);
    if (player_check_map_collision(map, &player_rect, &distance)) {
        // Fix player direction during collision
        if (player.dx < 0) {
            // Move up
            player.facing = PLAYER_FACING_LEFT;
        } else if (player.dx > 0){
            // Move down
            player.facing = PLAYER_FACING_RIGHT;
        }
        log_debug("Collision detected at x: %d y: %d h: %d w: %d", player.x, player.y, distance.h, distance.w);
        player.dy = PLAYER_SPEED - distance.h;
    }
    // Map object collision check
    map_check_object_collisions(map, "warp", &player_rect, collision_callback, map);

    // Keep player in the center of the camera until the camera hits the edge of the map
    // X Axis movement
    
    player.x += player.dx;
    player.y += player.dy;

    // Debug camera position
    // log_debug("Camera x: %f y: %f", camera->x, camera->y);

    // Prevent player from moving outside of map
    if (player.x < 0) {
        player.x = 0;
    }
    if (player.y < 0) {
        player.y = 0;
    }
    if (player.x > map_width - player.width) {
        player.x = map_width - player.width;
    }
    if (player.y > map_height - player.height) {
        player.y = map_height - player.height;
    }
    // Reset player dx and dy
    player.dx = 0;
    player.dy = 0;
    // Debug player position
    //log_debug("Player x: %d y: %d", player.x, player.y);
}

void player_draw(App * app)
{
	// Draw player
    Camera * camera = app->camera;
    tileset_render_tile(app, player.tileset, player.facing, true, player.x - camera->x, player.y - camera->y, player.move_speed > 0 ? 1 : 0);
}

void player_free() {
    // Free any memory allocated for the player
    // Free any memory allocated for the player's texture
    // Free any memory allocated for the player's animation
    // Free the player
    
}