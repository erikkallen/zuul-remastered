#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <cjson/cJSON.h>
#include <stdio.h>
#include <stdlib.h>
#include "structs.h"
#include "defs.h"
#include "draw.h"
#include "map.h"

// Logging
#include <log.h>

#define MAP_NUM_TILES 2
#define MAP_TILE_WIDTH 64
#define MAP_TILE_HEIGHT 64
#define MAP_ANIMATION_FRAMES 2
// Bits on the far end of the 32-bit global tile ID are used for tile flags
#define FLIPPED_HORIZONTALLY_FLAG  0x80000000
#define FLIPPED_VERTICALLY_FLAG    0x40000000
#define FLIPPED_DIAGONALLY_FLAG    0x20000000
#define ROTATED_HEXAGONAL_120_FLAG 0x10000000
#define TILE_FLAG_MASK  0xf0000000
#define TILE_ID_MASK    0x0fffffff

int map_load(App * app, Map * map) {
    // Read map file into buffer
    FILE *fp = fopen("../assets/home.tmj", "r");
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
            log_debug("Tile id: %d flags: %x gid: %u", new_layer->data[tile_index].id, new_layer->data[tile_index].flags, global_tile_id);
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
void map_tiles_load(App * app, Map * map) {
    // Read map file into buffer
    FILE *fp = fopen("../assets/tiles.tsj", "r");
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
    cJSON *tile_json = cJSON_Parse(string);
    const cJSON *tile_width = NULL;
    const cJSON *tile_height = NULL;
    const cJSON *image = NULL;
    const cJSON *tilecount = NULL;
    const cJSON *columns = NULL;
    const cJSON *image_height = NULL;
    const cJSON *image_width = NULL;
    const cJSON *spacing = NULL;
    const cJSON *margin = NULL;
    const cJSON *name = NULL;

    tile_width = cJSON_GetObjectItemCaseSensitive(tile_json, "tilewidth");
    if (!cJSON_IsNumber(tile_width)) {
        log_error("Failed to parse tile width");
        exit(1);
    }
    tile_height = cJSON_GetObjectItemCaseSensitive(tile_json, "tileheight");
    if (!cJSON_IsNumber(tile_height)) {
        log_error("Failed to parse tile height");
        exit(1);
    }
    image = cJSON_GetObjectItemCaseSensitive(tile_json, "image");
    if (!cJSON_IsString(image)) {
        log_error("Failed to parse image");
        exit(1);
    }
    tilecount = cJSON_GetObjectItemCaseSensitive(tile_json, "tilecount");
    if (!cJSON_IsNumber(tilecount)) {
        log_error("Failed to parse tilecount");
        exit(1);
    }
    columns = cJSON_GetObjectItemCaseSensitive(tile_json, "columns");
    if (!cJSON_IsNumber(columns)) {
        log_error("Failed to parse columns");
        exit(1);
    }
    image_height = cJSON_GetObjectItemCaseSensitive(tile_json, "imageheight");
    if (!cJSON_IsNumber(image_height)) {
        log_error("Failed to parse imageheight");
        exit(1);
    }
    image_width = cJSON_GetObjectItemCaseSensitive(tile_json, "imagewidth");
    if (!cJSON_IsNumber(image_width)) {
        log_error("Failed to parse imagewidth");
        exit(1);
    }
    spacing = cJSON_GetObjectItemCaseSensitive(tile_json, "spacing");
    if (!cJSON_IsNumber(spacing)) {
        log_error("Failed to parse spacing");
        exit(1);
    }
    margin = cJSON_GetObjectItemCaseSensitive(tile_json, "margin");
    if (!cJSON_IsNumber(margin)) {
        log_error("Failed to parse margin");
        exit(1);
    }
    name = cJSON_GetObjectItemCaseSensitive(tile_json, "name");
    if (!cJSON_IsString(name)) {
        log_error("Failed to parse name");
        exit(1);
    }

    strcpy(map->texture.filename, "../assets/");
    strcat(map->texture.filename, image->valuestring);
    log_info("Loaded tileset name: %s, tile width: %d, tile height: %d, tilecount: %d file: %s size: %d bytes", name->valuestring, tile_width->valueint, tile_height->valueint, tilecount->valueint, map->texture.filename, fsize);

    map->texture.frames = malloc(sizeof(SDL_Rect) * tilecount->valueint);
    for(int i = 0; i < tilecount->valueint; i++) {
        map->texture.frames[i].x = 0;
        map->texture.frames[i].y = i * tile_height->valueint;
        map->texture.frames[i].w = tile_width->valueint;
        map->texture.frames[i].h = tile_height->valueint;
    }
    map->texture.animation_speed = 250;
    int rc = draw_load_texture(app, &map->texture);
    if (rc != 0) {
        log_error("Failed to load texture");
        exit(1);
    }

    
    cJSON_Delete(tile_json);
    free(string);
}

void map_init(App *app, Map * map) {
    map_tiles_load(app, map);
    map_load(app, map);
}

void map_draw_layer(App * app, Map * map, Layer * layer) {
    // log_debug("Drawing layer w: %d h: %d p: %p, data_p: %p", layer->width, layer->height, layer, layer->data);
    for (int i = 0; i < layer->width; i++) {
        for (int j = 0; j < layer->height; j++) {
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
            SDL_Rect *src = &map->texture.frames[tile_id];
            dest.x = i * map->tile_width;
            dest.y = j * map->tile_height;
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

            SDL_RenderCopyEx(app->renderer, map->texture.texture, src, &dest, 0, NULL, flip);
        }
    }
}

void map_draw(App * app, Map * map) {
    //int ticks = SDL_GetTicks();
    // int sprite = (ticks / map->texture.animation_speed) % MAP_ANIMATION_FRAMES;
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
    free(map->texture.frames);
    SDL_DestroyTexture(map->texture.texture);
}