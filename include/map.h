#ifndef MAP_H
#define MAP_H

#include <stdint.h>
#include "structs.h"
#include "tileset.h"


typedef struct Chunk
{
    uint32_t* data; // Array of unsigned int (GIDs) or base64-encoded data
    int height; // Height in tiles
    int width; // Width in tiles
    int x; // X coordinate in tiles
    int y; // Y coordinate in tiles
} Chunk;

typedef struct Point
{
    double x; // X coordinate in pixels
    double y; // Y coordinate in pixels
} Point;

typedef struct Text
{
    bool bold; // Whether to use a bold font (default: false)
    char* color; // Hex-formatted color (#RRGGBB or #AARRGGBB) (default: #000000)
    char* fontfamily; // Font family (default: sans-serif)
    char* halign; // Horizontal alignment (center, right, justify or left (default))
    bool italic; // Whether to use an italic font (default: false)
    bool kerning; // Whether to use kerning when placing characters (default: true)
    int pixelsize; // Pixel size of font (default: 16)
    bool strikeout; // Whether to strike out the text (default: false)
    char* text; // Text
    bool underline; // Whether to underline the text (default: false)
    char* valign; // Vertical alignment (center, bottom or top (default))
    bool wrap; // Whether the text is wrapped within the object bounds (default: false)
} Text;


typedef struct Object
{
    bool ellipse; // Used to mark an object as an ellipse
    int gid; // Global tile ID, only if object represents a tile
    double height; // Height in pixels
    int id; // Incremental ID, unique across all objects
    char* name; // String assigned to name field in editor
    bool point; // Used to mark an object as a point
    Point* polygon; // Array of Points, in case the object is a polygon
    Point* polyline; // Array of Points, in case the object is a polyline
    Property* properties; // Array of Properties
    double rotation; // Angle in degrees clockwise
    char* template; // Reference to a template file, in case object is a template instance
    Text* text; // Only used for text objects
    char* type; // The class of the object (was saved as class in 1.9, optional)
    bool visible; // Whether object is shown in editor
    double width; // Width in pixels
    double x; // X coordinate in pixels
    double y; // Y coordinate in pixels
} Object;



typedef struct Layer
{
    uint32_t id;
    int32_t x;
    int32_t y;
    float opacity;
    // Additional fields
    Chunk* array; // Array of chunks optional
    uint32_t array_count; // Array of chunks optional
    char* class;
    char* compression;
    uint32_t* data;
    char* draworder;
    char* encoding;
    int height;
    int image;
    bool locked;
    struct Layer *layers; // Array of Layers
    uint32_t layer_count;
    char* name;
    struct Object* objects;
    double offsetx;
    double offsety;
    double parallaxx;
    double parallaxy;
    struct Property* properties;
    bool repeatx;
    bool repeaty;
    int startx;
    int starty;
    char* tintcolor;
    char* transparentcolor;
    char* type;
    bool visible;
    int width;
} Layer;

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

void map_init(App *app, Map *map, Tileset *tileset, const char *filename);
void map_draw(App *app, Map *map);
void map_free(Map *map);
uint32_t map_get_tile_id_at_x_y(Map * map, int layer_index, int x, int y);
uint32_t map_get_tile_id_at_row_col(Map * map, int layer_index, int row, int col) ;
Tile * map_get_tile_at(Map * map, int x, int y);
bool map_check_collision(Map * map, int col, int row);

#endif