#ifndef MAP_H
#define MAP_H

#include <stdint.h>
#include "structs.h"

typedef struct
{
    struct TextureImage texture;
    int width;
    int height;
    int tile_width;
    int tile_height;
    uint32_t *data;
} Map;

void map_init(App *app, Map *map);
void map_draw(App *app, Map *map);
void map_free(Map *map);

#endif