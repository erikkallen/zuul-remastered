#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <cjson/cJSON.h>
#include "tileset.h"
#include "structs.h"
#include "draw.h"

// Logging
#include <log.h>
/*
* Load a tileset from a json file and return a pointer to the loaded tileset
* 
* @param filename The filename of the tileset json file
* @return A pointer to the loaded tileset

*/
Tileset * tileset_load(const char * filename) {
    Tileset * tileset = calloc(1, sizeof(Tileset));
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
    tileset->tile_image.frames = calloc(tileset->num_tiles, sizeof(Frame));
    if (tileset->tile_image.frames == NULL) {
        log_error("Failed to allocate tile memory");
        exit(1);
    }
    tileset->tile_image.num_frames = tileset->num_tiles;
    tileset->tiles = calloc(tileset->num_tiles, sizeof(Tile));
    const cJSON *j_tile = NULL;
    int tile_index = 0;
    cJSON_ArrayForEach(j_tile, j_tiles) {
        Tile * tile = &tileset->tiles[tile_index];
        const cJSON * j_id = cJSON_GetObjectItemCaseSensitive(j_tile, "id");
        if (!cJSON_IsNumber(j_id)) {
            log_error("Failed to parse tile id");
            exit(1);
        }
        tile->id = j_id->valueint;
        const cJSON * j_image = cJSON_GetObjectItemCaseSensitive(j_tile, "image");
        if (cJSON_IsString(j_image)) {
            tile->image = calloc(strlen(j_image->valuestring) + 1, sizeof(char));
            strcpy(tile->image, j_image->valuestring);
        }

        const cJSON * j_imageheight = cJSON_GetObjectItemCaseSensitive(j_tile, "imageheight");
        if (!cJSON_IsNumber(j_imageheight)) {
            log_error("Failed to parse tile imageheight");
            exit(1);
        }
        tile->imageheight = j_imageheight->valueint;
        const cJSON * j_imagewidth = cJSON_GetObjectItemCaseSensitive(j_tile, "imagewidth");
        if (!cJSON_IsNumber(j_imagewidth)) {
            log_error("Failed to parse tile imagewidth");
            exit(1);
        }
        tile->imagewidth = j_imagewidth->valueint;
        const cJSON * j_type = cJSON_GetObjectItemCaseSensitive(j_tile, "type");
        if (cJSON_IsString(j_type)) {
            tile->type = calloc(strlen(j_type->valuestring) + 1, sizeof(char));
            strcpy(tile->type, j_type->valuestring);
        }
        const cJSON * j_properties = cJSON_GetObjectItemCaseSensitive(j_tile, "properties");
        if (cJSON_IsArray(j_properties)) {
            tile->property_count = cJSON_GetArraySize(j_properties);
            tile->properties = calloc(tile.property_count, sizeof(Property));
            const cJSON * j_property = NULL;
            int property_index = 0;
            cJSON_ArrayForEach(j_property, j_properties) {
                Property * property = &tile.properties[property_index];
                const cJSON * j_name = cJSON_GetObjectItemCaseSensitive(j_property, "name");
                if (!cJSON_IsString(j_name)) {
                    log_error("Failed to parse tile property name");
                    exit(1);
                }
                property->name = calloc(strlen(j_name->valuestring) + 1, sizeof(char));
                strcpy(property->name, j_name->valuestring);

                const cJSON * j_type = cJSON_GetObjectItemCaseSensitive(j_property, "type");
                if (!cJSON_IsString(j_type)) {
                    log_error("Failed to parse tile property type");
                    exit(1);
                }
                property->type = calloc(strlen(j_type->valuestring) + 1, sizeof(char));
                strcpy(property->type, j_type->valuestring);

                const cJSON * j_propertytype = cJSON_GetObjectItemCaseSensitive(j_property, "propertytype");
                if (!cJSON_IsString(j_propertytype)) {
                    log_error("Failed to parse tile property propertytype");
                    exit(1);
                }
                property->propertytype = calloc(strlen(j_propertytype->valuestring) + 1, sizeof(char));
                strcpy(property->propertytype, j_propertytype->valuestring);

                const cJSON * j_value = cJSON_GetObjectItemCaseSensitive(j_property, "value");
                if (cJSON_IsString(j_value)) {
                    property->string_value = calloc(strlen(j_value->valuestring) + 1, sizeof(char));
                    strcpy(property->value, j_value->valuestring);
                } else if (cJSON_IsNumber(j_value)) {
                    property->int_value = j_value->valueint;
                    property->float_value = j_value->valuedouble;
                } else if (cJSON_IsBool(j_value)) {
                    property->bool_value = j_value->valueint;
                } else {
                    log_warn("Property type not supported");
                } 
                property_index++;
            }
        }

        const cJSON * j_animation = cJSON_GetObjectItemCaseSensitive(j_tile, "animation");
        // Handle animation
        if (cJSON_IsArray(j_animation)) {
            // Allocate animation memory
            tile->animation_count = cJSON_GetArraySize(j_animation);
            tile->animation = calloc(tile->animation_count, sizeof(Frame));
            if (tileset->animation == NULL) {
                log_error("Failed to allocate animation memory");
                exit(1);
            }

            const cJSON *j_frame = NULL;
            size_t animation_index = 0;
            cJSON_ArrayForEach(j_frame, j_animation) {
                Frame * frame = &tile->animation[animation_index];
                const cJSON *j_duration = cJSON_GetObjectItemCaseSensitive(j_frame, "duration");
                if (!cJSON_IsNumber(j_duration)) {
                    log_error("Failed to parse tile duration");
                    exit(1);
                }
                const cJSON *j_tileid = cJSON_GetObjectItemCaseSensitive(j_frame, "tileid");
                if (!cJSON_IsNumber(j_tileid)) {
                    log_error("Failed to parse tile tileid");
                    exit(1);
                }
                frame->duration = j_duration->valueint;
                frame->tileid = j_tileid->valueint;
                animation_index++;
            }
        }
        tile_index++;
    }

    log_info("Loaded tileset name: %s, tile width: %d, tile height: %d, tilecount: %d file: %s size: %d bytes", j_name->valuestring, j_tile_width->valueint, j_tile_height->valueint, j_tilecount->valueint, filename, fsize);
    tileset->rows = tileset->num_tiles / tileset->columns;
    
    cJSON_Delete(tile_json);
    free(string);
}

void tileset_render_tile(App * app, Tileset * tileset, int global_tile_id, int x, int y) {
    SDL_Rect src;
    SDL_Rect dest;
    int tile_width = tileset->tile_width;
    int tile_height = tileset->tile_height;
    int columns = tileset->columns;
    int margin = tileset->margin;
    int spacing = tileset->spacing;
    int rows = tileset->rows;
    int tile_x = tileid % columns;
    int tile_y = tileid / columns;
    int tile_x_px = tile_x * (tile_width + spacing) + margin;
    int tile_y_px = tile_y * (tile_height + spacing) + margin;
    uint32_t tileid = global_tile_id & TILE_ID_MASK;
    Tile * tile = tileset->tiles[tile_id];
    SDL_Texture *texture = tileset->texture;

    // Setup atlasimage source and destination
    src.x = tile_x_px;
    src.y = tile_y_px;
    src.w = tile_width;
    src.h = tile_height;
    dest.x = x;
    dest.y = y;
    dest.w = tile_width;
    dest.h = tile_height;

    // Check if tile is animated
    if (tile->animation != NULL) {
        src = &frames[tileset_get_animation_frame_id(tile)].frame;
    }

    SDL_RendererFlip flip = SDL_FLIP_NONE;
    uint32_t flags = global_tile_id & TILE_FLAG_MASK;
    // Read flip from tile flags
    if (flags & FLIPPED_HORIZONTALLY_FLAG) {
        flip |= SDL_FLIP_HORIZONTAL;
    }
    if (flags & FLIPPED_VERTICALLY_FLAG) {
        flip |= SDL_FLIP_VERTICAL;
    }
    if (flags & FLIPPED_DIAGONALLY_FLAG) {
        flip |= SDL_FLIP_HORIZONTAL;
        flip |= SDL_FLIP_VERTICAL;
    }

    SDL_RenderCopyEx(app->renderer, texture, &src, &dest, 0, NULL, flip);
}

uint32_t tileset_get_animation_frame_id(Tile * tile) {
    // Get current frame from somewhere
    // Store last tick somewhere
    uint32_t duration = tile->animation[current_frame].duration;
    if (duration == 0) {
        log_error("Duration is 0");
        return 0;
    }
    int ticks = SDL_GetTicks();
    
    if (ticks - frame->animation->last_tick > duration) {
        // log_debug("Animating frame %d of %d", frame->animation->current_frame, frame->animation->num_frames);
        frame->animation->current_frame = (frame->animation->current_frame + 1) % tile->animation_count;
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
    // Free tileset tiles
    for (int i = 0; i < tiles->num_tiles; i++) {
        free(tiles->tiles[i].image);
        free(tiles->tiles[i].type);
        // Free properties
        for (int j = 0; j < tiles->tiles[i].property_count; j++) {
            free(tiles->tiles[i].properties[j].name);
            free(tiles->tiles[i].properties[j].type);
            free(tiles->tiles[i].properties[j].propertytype);
            if (tiles->tiles[i].properties[j].string_value != NULL) {
                free(tiles->tiles[i].properties[j].string_value);
            }
        }
        free(tiles->tiles[i].properties);
        // Free animation
        if (tiles->tiles[i].animation != NULL) {
            for (int j = 0; j < tiles->tiles[i].animation_count; j++) {
                free(tiles->tiles[i].animation[j].frames);
            }
            free(tiles->tiles[i].animation);
        }
    }
    SDL_DestroyTexture(tiles->texture);
    free(tiles->tiles);
    free(tiles);
}