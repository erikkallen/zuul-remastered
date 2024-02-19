#ifndef MAP_H
#define MAP_H

#include <stdint.h>
#include "structs.h"
#include "tileset.h"

typedef struct Map
{
    Tileset *tileset;
    int width;
    int height;
    char *backgroundcolor; // Hex-formatted color (#RRGGBB or #AARRGGBB) (optional)
    char *class; // The class of the map (since 1.9, optional)
    int compressionlevel; // The compression level to use for tile layer data (defaults to -1)
    int hexsidelength; // Length of the side of a hex tile in pixels (hexagonal maps only)
    bool infinite; // Whether the map has infinite dimensions
    Layer *layers; // Array of Layers
    uint32_t layer_count;
    int nextlayerid; // Auto-increments for each layer
    int nextobjectid; // Auto-increments for each placed object
    char *orientation; // orthogonal, isometric, staggered or hexagonal
    double parallaxoriginx; // X coordinate of the parallax origin in pixels (since 1.8, default: 0)
    double parallaxoriginy; // Y coordinate of the parallax origin in pixels (since 1.8, default: 0)
    Property *properties; // Array of Properties
    char *renderorder; // right-down (the default), right-up, left-down or left-up
    char *staggeraxis; // x or y (staggered / hexagonal maps only)
    char *staggerindex; // odd or even (staggered / hexagonal maps only)
    char *tiledversion; // The Tiled version used to save the file
    int tileheight; // Map grid height
    Tileset *tilesets; // Array of Tilesets
    int tilewidth; // Map grid width
    char *type; // map (since 1.0)
    char *version; // The JSON format version (previously a number, saved as string since 1.6)
} Map;

void map_init(Map *map, Tileset *tileset, const char *filename);
void map_draw(App *app, Map *map);
void map_free(Map *map);
int map_load(Map * map, const char *filename);
uint32_t map_get_tile_id_at_x_y(Map * map, int layer_index, int x, int y);
uint32_t map_get_tile_id_at_row_col(Map * map, int layer_index, int row, int col) ;
Tile * map_get_tile_at(Map * map, int x, int y);
bool map_check_tile_collision(Map * map, int col, int row, SDL_Rect * bb_rect, SDL_Rect * intersection);
bool map_check_object_collisions(Map * map, const char * name, SDL_Rect * player_rect, void (*collision_callback)(Property * property, void * data), void* data) ;
#endif