#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <cjson/cJSON.h>
#include "tileset.h"
#include "structs.h"
#include "draw.h"

// Logging
#include <log.h>


static void tileset_parse_properties(const cJSON *j_tile, Tile * tile) {
    const cJSON * j_properties = cJSON_GetObjectItemCaseSensitive(j_tile, "properties");
    tile->property_count = cJSON_GetArraySize(j_properties);
    tile->properties = NULL;
    if (cJSON_IsArray(j_properties)) {
        tile->properties = calloc(tile->property_count, sizeof(Property));
        const cJSON * j_property = NULL;
        int property_index = 0;
        cJSON_ArrayForEach(j_property, j_properties) {
            Property * property = &tile->properties[property_index];
            const cJSON * j_name = cJSON_GetObjectItemCaseSensitive(j_property, "name");
            if (!cJSON_IsString(j_name)) {
                log_error("Failed to parse tile property name");
                exit(1);
            }
            property->name = calloc(strlen(j_name->valuestring) + 1, sizeof(char));
            strcpy(property->name, j_name->valuestring);

            const cJSON * j_type = cJSON_GetObjectItemCaseSensitive(j_property, "type");
            if (cJSON_IsString(j_type)) {
                property->type = calloc(strlen(j_type->valuestring) + 1, sizeof(char));
                strcpy(property->type, j_type->valuestring);
            }

            const cJSON * j_propertytype = cJSON_GetObjectItemCaseSensitive(j_property, "propertytype");
            if (cJSON_IsString(j_propertytype)) {
                property->propertytype = calloc(strlen(j_propertytype->valuestring) + 1, sizeof(char));
                strcpy(property->propertytype, j_propertytype->valuestring);
            }
            

            const cJSON * j_value = cJSON_GetObjectItemCaseSensitive(j_property, "value");
            if (cJSON_IsString(j_value)) {
                property->string_value = calloc(strlen(j_value->valuestring) + 1, sizeof(char));
                strcpy(property->string_value, j_value->valuestring);
            } else if (cJSON_IsNumber(j_value)) {
                property->number_value = cJSON_GetNumberValue(j_value);
            } else if (cJSON_IsBool(j_value)) {
                property->bool_value = j_value->valueint;
            } else {
                log_warn("Property type not supported");
            } 
            property_index++;
        }
    }
}

static void tileset_parse_objectgroup(const cJSON *j_tile, Tile * tile) {
    const cJSON * j_objectgroup = cJSON_GetObjectItemCaseSensitive(j_tile, "objectgroup");
    if (cJSON_IsObject(j_objectgroup)) {
        const cJSON * j_objects = cJSON_GetObjectItemCaseSensitive(j_objectgroup, "objects");
        tile->objectgroup_count = cJSON_GetArraySize(j_objects);
        tile->objectgroup = NULL;
        if (cJSON_IsArray(j_objects)) {
            const cJSON * j_object = NULL;
            tile->objectgroup = calloc(tile->objectgroup_count, sizeof(Layer));
            int object_index = 0;
            cJSON_ArrayForEach(j_object, j_objects) {
                Layer * object = &tile->objectgroup[object_index];
                const cJSON * j_x = cJSON_GetObjectItemCaseSensitive(j_object, "x");
                if (!cJSON_IsNumber(j_x)) {
                    log_error("Failed to parse object x");
                    exit(1);
                }
                object->x = j_x->valueint;
                const cJSON * j_y = cJSON_GetObjectItemCaseSensitive(j_object, "y");
                if (!cJSON_IsNumber(j_y)) {
                    log_error("Failed to parse object y");
                    exit(1);
                }
                object->y = j_y->valueint;
                const cJSON * j_width = cJSON_GetObjectItemCaseSensitive(j_object, "width");
                if (!cJSON_IsNumber(j_width)) {
                    log_error("Failed to parse object width");
                    exit(1);
                }
                object->width = j_width->valueint;
                const cJSON * j_height = cJSON_GetObjectItemCaseSensitive(j_object, "height");
                if (!cJSON_IsNumber(j_height)) {
                    log_error("Failed to parse object height");
                    exit(1);
                }
                object->height = j_height->valueint;
                const cJSON * j_type = cJSON_GetObjectItemCaseSensitive(j_object, "type");
                if (cJSON_IsString(j_type)) {
                    object->type = calloc(strlen(j_type->valuestring) + 1, sizeof(char));
                    strcpy(object->type, j_type->valuestring);
                }
                const cJSON * j_name = cJSON_GetObjectItemCaseSensitive(j_object, "name");
                if (cJSON_IsString(j_name)) {
                    object->name = calloc(strlen(j_name->valuestring) + 1, sizeof(char));
                    strcpy(object->name, j_name->valuestring);
                }
                const cJSON * j_visible = cJSON_GetObjectItemCaseSensitive(j_object, "visible");
                if (!cJSON_IsBool(j_visible)) {
                    object->visible = j_visible->valueint;
                }
            }
            object_index++;
        }
    }
}

static void tileset_parse_animation(const cJSON *j_tile, Tile * tile) {
    const cJSON * j_animation = cJSON_GetObjectItemCaseSensitive(j_tile, "animation");
    tile->animation_count = cJSON_GetArraySize(j_animation);
    tile->animation = NULL;
    // Handle animation
    if (cJSON_IsArray(j_animation)) {
        // Allocate animation memory
        tile->animation = calloc(tile->animation_count, sizeof(Frame));
        if (tile->animation == NULL) {
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
}
/*
* Load a tileset from a json file and return a pointer to the loaded tileset
* 
* @param filename The filename of the tileset json file
* @return A pointer to the loaded tileset

*/
Tileset * tileset_load(App * app, const char * filename) {
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

    tileset->tile_width = j_tile_width->valueint;
    tileset->tile_height = j_tile_height->valueint;
    tileset->tile_image_width = j_image_width->valueint;
    tileset->tile_image_height = j_image_height->valueint;
    tileset->num_tiles = j_tilecount->valueint;
    tileset->margin = j_margin->valueint;
    tileset->spacing = j_spacing->valueint;
    tileset->columns = j_columns->valueint;
    strcpy(tileset->name, j_name->valuestring);
    log_debug("Tileset columns: %d", tileset->columns);
    // Allocate tile memory
    const cJSON *j_tiles = cJSON_GetObjectItemCaseSensitive(tile_json, "tiles");
    if (cJSON_IsArray(j_tiles)) {
        tileset->tile_count = cJSON_GetArraySize(j_tiles);
        tileset->tiles = calloc(tileset->tile_count, sizeof(Tile));
        if (tileset->tiles == NULL) {
            log_error("Failed to allocate tile memory");
            exit(1);
        }
        const cJSON *j_tile = NULL;
        int tile_index = 0;
        cJSON_ArrayForEach(j_tile, j_tiles) {
            Tile * tile = &tileset->tiles[tile_index];
            const cJSON * j_id = cJSON_GetObjectItemCaseSensitive(j_tile, "id");
            if (!cJSON_IsNumber(j_id)) {
                log_error("Failed to parse tile id");
                exit(1);
            }
            log_debug("Loading tile id: %d", j_id->valueint);
            tile->id = j_id->valueint;
            const cJSON * j_image = cJSON_GetObjectItemCaseSensitive(j_tile, "image");
            if (cJSON_IsString(j_image)) {
                tile->image = calloc(strlen(j_image->valuestring) + 1, sizeof(char));
                strcpy(tile->image, j_image->valuestring);
            }

            const cJSON * j_imageheight = cJSON_GetObjectItemCaseSensitive(j_tile, "imageheight");
            if (cJSON_IsNumber(j_imageheight)) {
                tile->imageheight = j_imageheight->valueint;
            }
            const cJSON * j_imagewidth = cJSON_GetObjectItemCaseSensitive(j_tile, "imagewidth");
            if (cJSON_IsNumber(j_imagewidth)) {
                tile->imagewidth = j_imagewidth->valueint;
            }
            const cJSON * j_type = cJSON_GetObjectItemCaseSensitive(j_tile, "type");
            if (cJSON_IsString(j_type)) {
                tile->type = calloc(strlen(j_type->valuestring) + 1, sizeof(char));
                strcpy(tile->type, j_type->valuestring);
            }
            // Handle properties
            tileset_parse_properties(j_tile, tile);
            // Handle objectgroup
            tileset_parse_objectgroup(j_tile, tile);
            // Handle animation
            tileset_parse_animation(j_tile, tile);
            
            tile_index++;
        }
    }

    log_info("Loaded tileset name: %s, tile width: %d, tile height: %d, tilecount: %d file: %s size: %d bytes", j_name->valuestring, j_tile_width->valueint, j_tile_height->valueint, j_tilecount->valueint, filename, fsize);
    tileset->rows = tileset->num_tiles / tileset->columns;
    // Load tileset texture
    char texture_path[256];
    strcpy(texture_path, app->assets_path);
    strcat(texture_path, j_image->valuestring);
    log_info("Loading tileset texture: %s", j_image->valuestring);
    tileset->texture = IMG_LoadTexture(app->renderer, texture_path);
    SDL_assert(tileset->texture != NULL);
    
    cJSON_Delete(tile_json);
    free(string);
    return tileset;
}

static uint32_t tileset_get_current_animation_tileid(Tile * tile) {
    // Get current frame from somewhere
    // Store last tick somewhere
    uint32_t duration = tile->animation[tile->current_animation_frame].duration;
    if (duration == 0) {
        log_error("Duration is 0");
        return 0;
    }
    int ticks = SDL_GetTicks();
    
    if (ticks - tile->last_tick > duration) {
        // log_debug("Animating frame %d of %d id ", tile->current_animation_frame, tile->animation_count);
        tile->current_animation_frame = (tile->current_animation_frame + 1) % tile->animation_count;
        tile->last_tick = ticks;
    }
    return tile->animation[tile->current_animation_frame].tileid;
}

Tile * tileset_get_tile_by_id(Tileset * tileset, int tile_id, bool local) {
    uint32_t local_tile_id = tile_id;
    if (!local) {
        local_tile_id = (tile_id & TILE_ID_MASK) - 1;
    }
    for (int i = 0; i < tileset->tile_count; i++) {
        if (tileset->tiles[i].id == local_tile_id) {
            return tileset->tiles + i;
        }
    }
    return NULL;
}

void tileset_render_tile(App * app, Tileset * tileset, int tile_id,bool local_tile_id, int x, int y, bool animated) {
    if (tile_id == 0 && !local_tile_id) {
        // Skip rendering global empty tiles
        return;
    }
    uint32_t flags = 0;
    uint32_t tileid = tile_id;
    Tile * tile = NULL;
    if (!local_tile_id) {
        flags = tile_id & TILE_FLAG_MASK;
        tileid = (tile_id & TILE_ID_MASK) - 1;
    } 

    tile = tileset_get_tile_by_id(tileset, tileid, true);
    
    
    if (tile != NULL) {
        // Check if tile is animated
        if (tile->animation != NULL && animated) {
            tileid = tileset_get_current_animation_tileid(tile);
        }
    }
    // log_debug("Rendering tile %d pos: [%d %d] animated: %d", tileid, x, y, animated);
    int columns = tileset->columns;
    int tile_x = tileid % columns;
    int tile_y = tileid / columns;
    SDL_Rect src;
    SDL_Rect dest;
    int tile_width = tileset->tile_width;
    int tile_height = tileset->tile_height;
    
    int margin = tileset->margin;
    int spacing = tileset->spacing;
    int tile_x_px = tile_x * (tile_width + spacing) + margin;
    int tile_y_px = tile_y * (tile_height + spacing) + margin;
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

    SDL_RendererFlip flip = SDL_FLIP_NONE;
    
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
    // log_debug("Rendering tile %d pos src: [%d %d] dst: [%d %d] col: %d", tileid, tile_x_px, tile_y_px, x, y, columns);

    SDL_RenderCopyEx(app->renderer, texture, &src, &dest, 0, NULL, flip);
}

void tileset_free(Tileset * tiles) {
    // Free tileset tiles
    for (int i = 0; i < tiles->num_tiles; i++) {
        free(tiles->tiles[i].image);
        free(tiles->tiles[i].type);
        // Free properties
        for (int j = 0; j < tiles->tiles[i].property_count; j++) {
            if (tiles->tiles[i].properties[j].name != NULL) {
                free(tiles->tiles[i].properties[j].name);
            }
            if (tiles->tiles[i].properties[j].type != NULL) {
                free(tiles->tiles[i].properties[j].type);
            }
            if (tiles->tiles[i].properties[j].propertytype != NULL) {
                free(tiles->tiles[i].properties[j].propertytype);
            }
            if (tiles->tiles[i].properties[j].string_value != NULL) {
                free(tiles->tiles[i].properties[j].string_value);
            }
        }
        if (tiles->tiles[i].properties != NULL) {
            free(tiles->tiles[i].properties);
        }
        // Free animation
        if (tiles->tiles[i].animation != NULL) {
            free(tiles->tiles[i].animation);
        }
        // Free objectgroup
        if (tiles->tiles[i].objectgroup != NULL) {
            for (int j = 0; j < tiles->tiles[i].objectgroup_count; j++) {
                if (tiles->tiles[i].objectgroup[j].name != NULL) {
                    free(tiles->tiles[i].objectgroup[j].name);
                }
                if (tiles->tiles[i].objectgroup[j].type != NULL) {
                    free(tiles->tiles[i].objectgroup[j].type);
                }
            }
            free(tiles->tiles[i].objectgroup);
        }
    }
    SDL_DestroyTexture(tiles->texture);
    free(tiles->tiles);
    free(tiles);
}