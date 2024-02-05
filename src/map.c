#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <cjson/cJSON.h>
#include <stdio.h>
#include <stdlib.h>
#include "structs.h"
#include "defs.h"
#include "draw.h"
#include "map.h"
#include "tileset.h"

// Logging
#include <log.h>

#define MAP_NUM_TILES 2
#define MAP_TILE_WIDTH 64
#define MAP_TILE_HEIGHT 64
#define MAP_ANIMATION_FRAMES 2


int map_load(App * app, Map * map, const char *filename) {
    // Read map file into buffer
    FILE *fp = fopen(filename, "r");
    if (fp == NULL) {
        log_error("Failed to open map file");
        exit(1);
    }
    fseek(fp, 0, SEEK_END);
    long fsize = ftell(fp);
    fseek(fp, 0, SEEK_SET);  /* same as rewind(f); */

    char *string = malloc(fsize + 1);
    fread(string, fsize, 1, fp);
    fclose(fp);

    string[fsize] = 0;

    // Load json map data
    cJSON *map_json = cJSON_Parse(string);
    const cJSON *height = NULL;
    const cJSON *width = NULL;
    const cJSON *tile_width = NULL;
    const cJSON *tile_height = NULL;
    const cJSON *layer = NULL;
    const cJSON *layers = NULL;
    const cJSON *tile = NULL;
    height = cJSON_GetObjectItemCaseSensitive(map_json, "height");
    if (!cJSON_IsNumber(height)) {
        log_error("Failed to parse map height");
        exit(1);
    }
    width = cJSON_GetObjectItemCaseSensitive(map_json, "width");
    if (!cJSON_IsNumber(width)) {
        log_error("Failed to parse map width");
        exit(1);
    }

    tile_width = cJSON_GetObjectItemCaseSensitive(map_json, "tilewidth");
    if (!cJSON_IsNumber(tile_width)) {
        log_error("Failed to parse map tilewidth");
        exit(1);
    }
    tile_height = cJSON_GetObjectItemCaseSensitive(map_json, "tileheight");
    if (!cJSON_IsNumber(tile_height)) {
        log_error("Failed to parse map tileheight");
        exit(1);
    }
    layers = cJSON_GetObjectItemCaseSensitive(map_json, "layers");
    if (!cJSON_IsArray(layers)) {
        log_error("Failed to parse map layers");
        exit(1);
    }
    map->width = width->valueint;
    map->height = height->valueint;
    map->tile_width = tile_width->valueint;
    map->tile_height = tile_height->valueint;
    map->layer = NULL;
    uint32_t num_layers = cJSON_GetArraySize(layers);
    
    log_debug("Parsing layers");
    cJSON_ArrayForEach(layer, layers) {
        const cJSON *data = NULL;
        const cJSON *width = NULL;
        const cJSON *height = NULL;
        // Read json data
        data = cJSON_GetObjectItemCaseSensitive(layer, "data");
        if (!cJSON_IsArray(data)) {
            log_error("Failed to parse map data");
            exit(1);
        }
        width = cJSON_GetObjectItemCaseSensitive(layer, "width");
        if (!cJSON_IsNumber(width)) {
            log_error("Failed to parse layer width");
            exit(1);
        }
        height = cJSON_GetObjectItemCaseSensitive(layer, "height");
        if (!cJSON_IsNumber(height)) {
            log_error("Failed to parse layer height");
            exit(1);
        }

        // Allocate layer
        log_debug("Allocating layer");
        // Set layer pointer to next layer
        Layer * new_layer = calloc(1, sizeof(Layer));
        
        if (new_layer == NULL) {
            log_error("Failed to allocate map layer");
            exit(1);
        }

        new_layer->width = width->valueint;
        new_layer->height = height->valueint;

        // Allocate tiles for layer
        new_layer->data = calloc(new_layer->width * new_layer->height, sizeof(Tile));
        if (new_layer->data == NULL) {
            log_error("Failed to allocate map data");
            exit(1);
        }
        int tile_index = 0;
        cJSON_ArrayForEach(tile, data) {
            if (!cJSON_IsNumber(tile)) {
                log_error("Failed to parse map tile");
                exit(1);
            }

            // Get global tile id and map to local tile id
            uint32_t global_tile_id = (uint32_t)tile->valuedouble;
                // Read out the flags
            // bool flipped_horizontally = (global_tile_id & FLIPPED_HORIZONTALLY_FLAG);
            // bool flipped_vertically = (global_tile_id & FLIPPED_VERTICALLY_FLAG);
            // bool flipped_diagonally = (global_tile_id & FLIPPED_DIAGONALLY_FLAG);
            // bool rotated_hex120 = (global_tile_id & ROTATED_HEXAGONAL_120_FLAG);
            
            // Read flags
            new_layer->data[tile_index].flags = global_tile_id & TILE_FLAG_MASK;
            new_layer->data[tile_index].id = (global_tile_id & TILE_ID_MASK);
            // log_debug("Tile id: %d flags: %x gid: %u", new_layer->data[tile_index].id, new_layer->data[tile_index].flags, global_tile_id);
            tile_index++;
        }
        // Move pointer to next layer
        if (map->layer == NULL) {
            log_debug("Setting head");
            map->layer = new_layer;
        } else {
            Layer * p = map->layer;
            while (p->next != NULL) {
                p = p->next;
            }
            log_debug("Setting next");
            p->next = new_layer;
        }
        log_info("Loaded layer width: %d, height: %d number_of_tiles: %d", width->valueint, height->valueint, map->width * map->height);
        log_debug("Next layer");
    }
    
    log_info("Loaded map width: %d, height: %d number_of_layers: %d", width->valueint, height->valueint, num_layers);

    cJSON_Delete(map_json);
    free(string);
    return 0;
}
// Load map tiles using tiles.tsj json file for meta info
/* 
example data
{ "columns":2,
 "image":"..\/tiles.png",
 "imageheight":128,
 "imagewidth":64,
 "margin":0,
 "name":"tiles",
 "spacing":0,
 "tilecount":8,
 "tiledversion":"1.10.2",
 "tileheight":32,
 "tilewidth":32,
 "type":"tileset",
 "version":"1.10"
}
*/

void map_init(App *app, Map * map, Tileset * tileset, const char *filename) {
    map->tileset = tileset;
    map_load(app, map, filename);
}

void map_draw_layer(App * app, Map * map, Layer * layer) {
    // Calculate start and end col and row pased on camera position
    int32_t start_col = app->camera->x / map->tile_width;
    int32_t start_row = app->camera->y / map->tile_height;
    int32_t end_col = start_col + (app->camera->width / map->tile_width) + 1;
    int32_t end_row = start_row + (app->camera->height / map->tile_height) + 1;

    uint32_t offset_x = -app->camera->x + start_col * map->tile_width;
    uint32_t offset_y = -app->camera->y + start_row * map->tile_height;
    // Check map bound and adjust end col and row
    if (end_col > layer->width) {
        end_col = layer->width;
        offset_x = 0;
    }
    if (end_row > layer->height) {
        end_row = layer->height;
        offset_y = 0;
    }
    if (start_col < 0) {
        start_col = 0;
        offset_x = 0;
    }
    if (start_row < 0) {
        start_row = 0;
        offset_y = 0;
    }
    // log_debug("Drawing layer w: %d h: %d p: %p, data_p: %p", layer->width, layer->height, layer, layer->data);
    log_debug("Drawing layer start_col: %d end_col: %d start_row: %d end_row: %d", start_col, end_col, start_row, end_row);
    for (int i = start_col; i < end_col; i++) {
        for (int j = start_row; j < end_row; j++) {
            SDL_Rect dest;
            uint32_t tile_index = i+(j*layer->width);
            if (tile_index > layer->width * layer->height) {
                log_error("Tile index out of range");
                continue;
            }
            // Skip tile_index -1
            if (layer->data[tile_index].id == 0) {
                continue;
            }
            // Convert to local tile index TODO make work for multiple tilesets
            uint32_t tile_id = layer->data[tile_index].id - 1;
            // log_debug("Drawing tile %d", tile_id);
            Frame * frames = map->tileset->tile_image.frames;
            Frame * frame = &map->tileset->tile_image.frames[tile_id];
            SDL_Texture *texture = map->tileset->tile_image.texture;
            SDL_Rect * src = &frame->frame;
            if (frame->animation != NULL) {
                src = &frames[tileset_get_animation_frame_id(frame)].frame;
            }
            dest.x = (i - start_col) * map->tile_width + offset_x;
            dest.y = (j - start_row) * map->tile_height + offset_y;
            dest.w = map->tile_width;
            dest.h = map->tile_height;
            SDL_RendererFlip flip = SDL_FLIP_NONE;
            // Read flip from tile flags
            if (layer->data[tile_index].flags & FLIPPED_HORIZONTALLY_FLAG) {
                flip |= SDL_FLIP_HORIZONTAL;
            }
            if (layer->data[tile_index].flags & FLIPPED_VERTICALLY_FLAG) {
                flip |= SDL_FLIP_VERTICAL;
            }
            if (layer->data[tile_index].flags & FLIPPED_DIAGONALLY_FLAG) {
                flip |= SDL_FLIP_HORIZONTAL;
                flip |= SDL_FLIP_VERTICAL;
            }

            SDL_RenderCopyEx(app->renderer, texture, src, &dest, 0, NULL, flip);
        }
    }
}

void map_draw(App * app, Map * map) {
    // Loop thourgh all layers and draw tiles
    // log_debug("Drawing map %p", map->layer);
    Layer * current_layer = map->layer;
    while (current_layer != NULL) {
        map_draw_layer(app, map, current_layer);
        current_layer = current_layer->next;
    }
}

void map_free(Map * map) {
    // Free all layers
    Layer * current_layer = map->layer;
    while (current_layer != NULL) {
        free(current_layer->data);
        Layer * next_layer = current_layer->next;
        free(current_layer);
        current_layer = next_layer;
    }
}