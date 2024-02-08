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
    
    
    const cJSON * j_height = cJSON_GetObjectItemCaseSensitive(map_json, "height");
    if (!cJSON_IsNumber(j_height)) {
        log_error("Failed to parse map height");
        exit(1);
    }
    const cJSON * j_width = cJSON_GetObjectItemCaseSensitive(map_json, "width");
    if (!cJSON_IsNumber(j_width)) {
        log_error("Failed to parse map width");
        exit(1);
    }
    const cJSON * j_tile_width = cJSON_GetObjectItemCaseSensitive(map_json, "tilewidth");
    if (!cJSON_IsNumber(j_tile_width)) {
        log_error("Failed to parse map tilewidth");
        exit(1);
    }
    const cJSON * j_tile_height = cJSON_GetObjectItemCaseSensitive(map_json, "tileheight");
    if (!cJSON_IsNumber(j_tile_height)) {
        log_error("Failed to parse map tileheight");
        exit(1);
    }
    const cJSON * j_layers = cJSON_GetObjectItemCaseSensitive(map_json, "layers");
    if (!cJSON_IsArray(j_layers)) {
        log_error("Failed to parse map layers");
        exit(1);
    }
    map->width = j_width->valueint;
    map->height = j_height->valueint;
    map->tilewidth = j_tile_width->valueint;
    map->tileheight = j_tile_height->valueint;
    map->layer_count = cJSON_GetArraySize(j_layers);
    map->layers = calloc(map->layer_count, sizeof(Layer));
    if (map->layers == NULL) {
        log_error("Failed to allocate map layers");
        exit(1);
    }
    log_debug("Parsing layers");
    const cJSON *j_layer;
    int layer_index = 0;
    cJSON_ArrayForEach(j_layer, j_layers) {
        // Read json data
        const cJSON *j_data = cJSON_GetObjectItemCaseSensitive(j_layer, "data");
        if (!cJSON_IsArray(j_data)) {
            log_error("Failed to parse map data");
            exit(1);
        }
        const cJSON *j_width = cJSON_GetObjectItemCaseSensitive(j_layer, "width");
        if (!cJSON_IsNumber(j_width)) {
            log_error("Failed to parse layer width");
            exit(1);
        }
        const cJSON *j_height = cJSON_GetObjectItemCaseSensitive(j_layer, "height");
        if (!cJSON_IsNumber(j_height)) {
            log_error("Failed to parse layer height");
            exit(1);
        }

        // Allocate layer
        log_debug("Allocating layer");
        // Set layer pointer to next layer
        Layer * layer = &map->layers[layer_index];
        
        layer->width = j_width->valueint;
        layer->height = j_height->valueint;

        // Allocate tiles for layer
        layer->data = calloc(layer->width * layer->height, sizeof(uint32_t));
        if (layer->data == NULL) {
            log_error("Failed to allocate map data");
            exit(1);
        }
        int data_index = 0;
        const cJSON *j_gid;
        cJSON_ArrayForEach(j_gid, j_data) {
            if (!cJSON_IsNumber(j_gid)) {
                log_error("Failed to parse map tile");
                exit(1);
            }

            // Read flags
            layer->data[data_index] = j_gid->valueint;
            data_index++;
        }
        layer_index++;
    }
    
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
    int32_t start_col = app->camera->x / map->tilewidth;
    int32_t start_row = app->camera->y / map->tileheight;
    int32_t end_col = start_col + ceil(app->camera->width / map->tilewidth) + 1;
    int32_t end_row = start_row + ceil(app->camera->height / map->tileheight) + 2;
    if (end_row > layer->height) {
        end_row = layer->height;
    }

    uint32_t offset_x = -app->camera->x + start_col * map->tilewidth;
    uint32_t offset_y = -app->camera->y + start_row * map->tileheight;
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
    // log_debug("Drawing layer start_col: %d end_col: %d start_row: %d end_row: %d", start_col, end_col, start_row, end_row);
    for (int i = start_col; i < end_col; i++) {
        for (int j = start_row; j < end_row; j++) {
            uint32_t tile_index = i+(j*layer->width);
            if (tile_index > layer->width * layer->height) {
                log_error("Tile index out of range");
                continue;
            }
            // Convert to local tile index TODO make work for multiple tilesets
            uint32_t global_tile_id = layer->data[tile_index];
            // log_debug("Drawing tile %d", tile_id);
            int x = (i - start_col) * map->tilewidth + offset_x;
            int y = (j - start_row) * map->tileheight + offset_y;
            tileset_render_tile(app, map->tileset, global_tile_id, x, y, true);
        }
    }
}

void map_draw(App * app, Map * map) {
    // Loop thourgh all layers and draw tiles
    // log_debug("Drawing map %d", map->tileset->columns);
    for (int i = 0; i < map->layer_count; i++) {
        map_draw_layer(app, map, &map->layers[i]);
    }
}

void map_free(Map * map) {
    // Free all layers
    for (int i = 0; i < map->layer_count; i++) {
        free(map->layers[i].data);
    }
    free(map->layers);
}

uint32_t map_get_tile_id_at_x_y(Map * map, int layer_index, int x, int y) {
    log_debug("Getting tile at %d %d", x, y);
    if (layer_index > map->layer_count) {
        log_error("Layer index out of range");
        return 0;
    }
    Layer * layer = &map->layers[layer_index];
    if (x < 0 || y < 0 || x > layer->width || y > layer->height) {
        log_error("Tile index out of range");
        return 0;
    }
    uint32_t tile_col = x / map->tilewidth;
    uint32_t tile_row = y / map->tileheight;
    uint32_t tile_index = tile_col + (tile_row * (layer->width / map->tilewidth));
    log_debug("Tile index %d", tile_index);
    return layer->data[tile_index];
}

uint32_t map_get_tile_id_at_row_col(Map * map, int layer_index, int col, int row) {
    // log_debug("Getting tile at %d %d", row, col);
    if (layer_index > map->layer_count) {
        log_error("Layer index out of range");
        return 0;
    }
    Layer * layer = &map->layers[layer_index];
    if (row < 0 || col < 0 || row > (layer->width) || row > (layer->height)) {
        log_error("Tile index out of range");
        return 0;
    }

    uint32_t tile_index = col + (row * (layer->width));
    //log_debug("Tile index %d layer", tile_index);
    return layer->data[tile_index];
}

Tile * map_get_tile_at(Map * map, int col, int row) {
    for (int i=map->layer_count-1;i>=0;i--) {
        uint32_t global_tile_id = map_get_tile_id_at_row_col(map, i, col, row);
        //log_debug("Global tile id: %d layer: %d", global_tile_id, i);
        if (global_tile_id != 0) {
            // Get tileset
            Tileset * tileset = map->tileset;
            // Get tileset tile
            Tile * tile = tileset_get_tile_by_id(tileset, global_tile_id, false);
            if (tile == NULL) {
                // log_error("Tile not found");
                continue;
            } else {
                return tile;
            }
        }
    }
    return NULL;
}

bool map_check_collision(Map * map, int col, int row) {
    Tile * tile = map_get_tile_at(map, col, row);
    if (tile == NULL) {
        return false;
    }
    for (int i=0;i<tile->property_count;i++) {
        if (strcmp(tile->properties[i].name, "solid") == 0) {
            return tile->properties[i].bool_value;
        }
    }
    return false;
}