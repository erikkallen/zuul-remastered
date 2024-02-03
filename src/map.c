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

    map->data = calloc(sizeof(int *), map->width * map->height);

    cJSON_ArrayForEach(layer, layers) {
        const cJSON *data = NULL;
        data = cJSON_GetObjectItemCaseSensitive(layer, "data");
        if (!cJSON_IsArray(data)) {
            log_error("Failed to parse map data");
            exit(1);
        }
        int tile_index = 0;
        cJSON_ArrayForEach(tile, data) {
            if (!cJSON_IsNumber(tile)) {
                log_error("Failed to parse map tile");
                exit(1);
            }

            // Get global tile id and map to local tile id
            // TODO Lookup tileset & handle flags
            int global_tile_id = tile->valueint;
            // log_debug("Tile[%d]: %d", tile_index, global_tile_id);
            map->data[tile_index] = global_tile_id - 1;
            tile_index++;
        }
    }
    log_info("Loaded map width: %d, height: %d", width->valueint, height->valueint);

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

    log_info("Loaded tileset name: %s, tile width: %d, tile height: %d, tilecount: %d", name->valuestring, tile_width->valueint, tile_height->valueint, tilecount->valueint);

    strcpy(map->texture.filename, "../assets/");
    strcat(map->texture.filename, image->valuestring);
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

void map_draw(App * app, Map * map) {
    int ticks = SDL_GetTicks();
    int sprite = 0;//(ticks / map->texture.animation_speed) % MAP_ANIMATION_FRAMES;
    for (int i = 0; i < map->width; i++) {
        for (int j = 0; j < map->height; j++) {
            SDL_Rect dest;
            uint32_t tile_id = map->data[i+(j*map->width)];
            // log_debug("Drawing tile %d", tile_id);
            SDL_Rect *src = &map->texture.frames[tile_id+sprite];
            dest.x = i * map->tile_width;
            dest.y = j * map->tile_height;
            dest.w = map->tile_width;
            dest.h = map->tile_height;
            SDL_RenderCopy(app->renderer, map->texture.texture, src, &dest);
        }
    }
}

void map_set_tile(Map * map, int x, int y, int tile) {
    *(map->data+x+y) = tile;
}

int map_get_tile(Map * map, int x, int y) {
    return *(map->data+x+y);
}

void map_free(Map * map) {
    free(map->data);
    free(map->texture.frames);
    SDL_DestroyTexture(map->texture.texture);
}