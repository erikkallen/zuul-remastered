#ifndef MAP_H
#define MAP_H

#include <stdint.h>
#include "structs.h"

typedef struct
{
    uint32_t id;
    uint32_t flags;
} Tile;
typedef struct Layer Layer; // Add forward declaration of Layer struct

typedef struct Layer
{
    uint32_t id;
    uint32_t x;
    uint32_t y;
    uint32_t width;
    uint32_t height;
    float opacity;
    Tile *data;
    Layer *next;
} Layer;

typedef struct
{
    struct TextureImage texture;
    int width;
    int height;
    int tile_width;
    int tile_height;
    Layer *layer;
} Map;

void map_init(App *app, Map *map);
void map_draw(App *app, Map *map);
void map_free(Map *map);

#endif