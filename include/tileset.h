#ifndef TILESET_H
#define TILESET_H

#include <stdint.h>
#include <SDL2/SDL.h>
#include <stdbool.h>

#include "defs.h"
#include "app.h"

// Bits on the far end of the 32-bit global tile ID are used for tile flags
#define FLIPPED_HORIZONTALLY_FLAG 0x80000000
#define FLIPPED_VERTICALLY_FLAG 0x40000000
#define FLIPPED_DIAGONALLY_FLAG 0x20000000
#define ROTATED_HEXAGONAL_120_FLAG 0x10000000
#define TILE_FLAG_MASK 0xf0000000
#define TILE_ID_MASK 0x0fffffff

/**
 * https://doc.mapeditor.org/en/stable/reference/json-map-format
 */

typedef enum TileType
{
    TILE_TYPE_WATER,
    TILE_TYPE_IMAGE,
    TILE_TYPE_OBJECT
} TileType;

typedef struct Animation
{
    uint32_t current_frame;
    uint32_t last_tick;
} Animation;

typedef struct Frame
{
    int duration;
    int tileid;
} Frame;

typedef struct RenderFrame
{
    uint32_t start_tick;
    int current_rect;
    uint32_t rect_count;
    SDL_Rect *rect;
} RenderFrame;
typedef struct AtlasImage
{
    char filename[MAX_FILENAME_LENGTH];
    uint32_t frame_count;
    RenderFrame *frames;
    
    SDL_Texture *texture;
} AtlasImage;

typedef struct Property
{
    char *name;
    char *propertytype;
    char *type;
    union
    {
        char *string_value;
        int int_value;
        double float_value;
        bool bool_value;
        char *color_value;
        char *file_value;
        int object_value;
    };
} Property;

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
    size_t property_count; // Number of Properties
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
    size_t object_count;
    struct Object* objects;
    double offsetx;
    double offsety;
    double parallaxx;
    double parallaxy;
    size_t property_count;
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

// Still deciding on how to best parse/render this
typedef struct Tile
{
    char *image; // Optional

    int id; // Local id
    int imageheight;
    int imagewidth;
    int x;
    int y;
    int width;
    int height;

    double probability; // Optional

    size_t animation_count;
    Frame *animation;

    Layer *objectgroup; // Optional
    size_t objectgroup_count;

    uint32_t current_animation_frame;
    uint32_t last_tick;

    int terrain[4]; // Optional
    char *type;     // Optional

    size_t property_count;
    Property *properties;
} Tile;

typedef struct Tileset
{
    uint32_t first_gid;
    char name[MAX_FILENAME_LENGTH];
    uint32_t columns;
    uint32_t rows;

    uint32_t tile_width;
    uint32_t tile_height;
    uint32_t tile_image_width;
    uint32_t tile_image_height;
    uint32_t spacing;
    uint32_t margin;
    uint32_t num_tiles;

    Tile *tiles;
    uint32_t tile_count;
    SDL_Texture *texture;
} Tileset;

Tileset * tileset_load(App * app, const char * filename);
void tileset_free(Tileset *tiles);
void tileset_render_tile(App * app, Tileset * tileset, int tileid,bool local_tile_id, int x, int y, bool animated);
Tile * tileset_get_tile_by_id(Tileset * tileset, int tile_id, bool local);
#endif