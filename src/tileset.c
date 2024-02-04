#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <cjson/cJSON.h>
#include "tileset.h"
#include "structs.h"
#include "draw.h"

// Logging
#include <log.h>

void tileset_load(App * app, Tileset * tileset, const char * filename) {
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
    cJSON *tile_json = cJSON_Parse(string);

    const cJSON *j_tile_width = cJSON_GetObjectItemCaseSensitive(tile_json, "tilewidth");
    if (!cJSON_IsNumber(j_tile_width)) {
        log_error("Failed to parse tile width");
        exit(1);
    }
    const cJSON *j_tile_height = cJSON_GetObjectItemCaseSensitive(tile_json, "tileheight");
    if (!cJSON_IsNumber(j_tile_height)) {
        log_error("Failed to parse tile height");
        exit(1);
    }
    const cJSON *j_image = cJSON_GetObjectItemCaseSensitive(tile_json, "image");
    if (!cJSON_IsString(j_image)) {
        log_error("Failed to parse image");
        exit(1);
    }
    const cJSON *j_tilecount = cJSON_GetObjectItemCaseSensitive(tile_json, "tilecount");
    if (!cJSON_IsNumber(j_tilecount)) {
        log_error("Failed to parse tilecount");
        exit(1);
    }
    const cJSON *j_columns = cJSON_GetObjectItemCaseSensitive(tile_json, "columns");
    if (!cJSON_IsNumber(j_columns)) {
        log_error("Failed to parse columns");
        exit(1);
    }
    const cJSON *j_image_height = cJSON_GetObjectItemCaseSensitive(tile_json, "imageheight");
    if (!cJSON_IsNumber(j_image_height)) {
        log_error("Failed to parse imageheight");
        exit(1);
    }
    const cJSON *j_image_width = cJSON_GetObjectItemCaseSensitive(tile_json, "imagewidth");
    if (!cJSON_IsNumber(j_image_width)) {
        log_error("Failed to parse imagewidth");
        exit(1);
    }
    const cJSON *j_spacing = cJSON_GetObjectItemCaseSensitive(tile_json, "spacing");
    if (!cJSON_IsNumber(j_spacing)) {
        log_error("Failed to parse spacing");
        exit(1);
    }
    const cJSON *j_margin = cJSON_GetObjectItemCaseSensitive(tile_json, "margin");
    if (!cJSON_IsNumber(j_margin)) {
        log_error("Failed to parse margin");
        exit(1);
    }
    const cJSON *j_name = cJSON_GetObjectItemCaseSensitive(tile_json, "name");
    if (!cJSON_IsString(j_name)) {
        log_error("Failed to parse name");
        exit(1);
    }
    const cJSON *j_tiles = cJSON_GetObjectItemCaseSensitive(tile_json, "tiles");
    if (!cJSON_IsArray(j_tiles)) {
        log_error("Failed to parse tiles");
        exit(1);
    }

    tileset->tile_width = j_tile_width->valueint;
    tileset->tile_height = j_tile_height->valueint;
    tileset->tile_image_width = j_image_width->valueint;
    tileset->tile_image_height = j_image_height->valueint;
    tileset->num_tiles = j_tilecount->valueint;
    tileset->margin = j_margin->valueint;
    tileset->spacing = j_spacing->valueint;
    tileset->columns = j_columns->valueint;
    strcpy(tileset->name, j_name->valuestring);

    // Allocate tile memory
    tileset->tile_image.frames = malloc(sizeof(Frame) * tileset->num_tiles);
    if (tileset->tile_image.frames == NULL) {
        log_error("Failed to allocate tile memory");
        exit(1);
    }
    tileset->tile_image.num_frames = tileset->num_tiles;

    const cJSON *j_tile = NULL;
    cJSON_ArrayForEach(j_tile, j_tiles) {
        const cJSON * j_id = cJSON_GetObjectItemCaseSensitive(j_tile, "id");
        if (!cJSON_IsNumber(j_id)) {
            log_error("Failed to parse tile id");
            exit(1);
        }
        const cJSON * j_animation = cJSON_GetObjectItemCaseSensitive(j_tile, "animation");
        // Handle animation
        if (cJSON_IsArray(j_animation)) {
            // Allocate animation memory
            tileset->tile_image.frames[j_id->valueint].animation = calloc(1, sizeof(Animation));
            if (tileset->tile_image.frames[j_id->valueint].animation == NULL) {
                log_error("Failed to allocate animation memory");
                exit(1);
            }
            // Allocate meory for animation frames
            tileset->tile_image.frames[j_id->valueint].animation->frames = calloc(cJSON_GetArraySize(j_animation), sizeof(AnimationFrame));
            tileset->tile_image.frames[j_id->valueint].animation->num_frames = cJSON_GetArraySize(j_animation);
            const cJSON *j_tile_a = NULL;
            size_t animation_index = 0;
            cJSON_ArrayForEach(j_tile_a, j_animation) {
                const cJSON *j_duration = NULL;
                const cJSON *j_tileid = NULL;
                j_duration = cJSON_GetObjectItemCaseSensitive(j_tile_a, "duration");
                if (!cJSON_IsNumber(j_duration)) {
                    log_error("Failed to parse tile duration");
                    exit(1);
                }
                j_tileid = cJSON_GetObjectItemCaseSensitive(j_tile_a, "tileid");
                if (!cJSON_IsNumber(j_tileid)) {
                    log_error("Failed to parse tile tileid");
                    exit(1);
                }
                tileset->tile_image.frames[j_id->valueint].animation->frames[animation_index].tileid = j_tileid->valueint;
                tileset->tile_image.frames[j_id->valueint].animation->frames[animation_index].duration = j_duration->valueint;
                animation_index++;
            }
        }

        const cJSON *j_type = cJSON_GetObjectItemCaseSensitive(j_tile, "type");
        // Handle type
    }

    log_info("Loaded tileset name: %s, tile width: %d, tile height: %d, tilecount: %d file: %s size: %d bytes", j_name->valuestring, j_tile_width->valueint, j_tile_height->valueint, j_tilecount->valueint, filename, fsize);
    tileset->rows = tileset->num_tiles / tileset->columns;

    // Set up frames
    for (int i = 0; i < tileset->columns; i++) {
        for (int j = 0; j < tileset->rows; j++) {
            tileset->tile_image.frames[i+(j*tileset->columns)].frame.x = i * tileset->tile_width;
            tileset->tile_image.frames[i+(j*tileset->columns)].frame.y = j * tileset->tile_height;
            tileset->tile_image.frames[i+(j*tileset->columns)].frame.w =     tileset->tile_width;
            tileset->tile_image.frames[i+(j*tileset->columns)].frame.h =     tileset->tile_height;
        }
    }
    char tileset_path[strlen(app->assets_path) + strlen(j_image->valuestring) + 1];
    strcpy(tileset_path, app->assets_path);
    strcat(tileset_path, j_image->valuestring);
    int rc = draw_load_texture(app, &tileset->tile_image, tileset_path);
    if (rc != 0) {
        log_error("Failed to load texture");
        exit(1);
    }

    
    cJSON_Delete(tile_json);
    free(string);
}

uint32_t tileset_get_animation_frame_id(Frame * frame) {
    uint32_t duration = frame->animation->frames[frame->animation->current_frame].duration;
    if (duration == 0) {
        log_error("Duration is 0");
        return 0;
    }
    int ticks = SDL_GetTicks();
    
    if (ticks - frame->animation->last_tick > duration) {
        // log_debug("Animating frame %d of %d", frame->animation->current_frame, frame->animation->num_frames);
        frame->animation->current_frame = (frame->animation->current_frame + 1) % frame->animation->num_frames;
        frame->animation->last_tick = ticks;
    }
    return frame->animation->frames[frame->animation->current_frame].tileid;
}

void tileset_free(Tileset * tiles) {
    // Free animations if they exist
    for (int i = 0; i < tiles->tile_image.num_frames; i++) {
        if (tiles->tile_image.frames[i].animation != NULL) {
            for (int j = 0; j < tiles->tile_image.frames[i].animation->num_frames; j++) {
                free(tiles->tile_image.frames[i].animation->frames);
            }
            free(tiles->tile_image.frames[i].animation);
        }
    }
    free(tiles->tile_image.frames);
    free(tiles->tile_image.texture);
    SDL_DestroyTexture(tiles->tile_image.texture);
}