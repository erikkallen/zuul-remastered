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

typedef struct AnimationFrame
{
    int tileid;
    int duration;
    struct AnimationFrame *next;
} AnimationFrame;

typedef struct Animation
{
    int num_frames;
    int frame_duration;
    int current_frame;
    uint32_t current_delay;
    uint32_t last_tick;
    // int loop;
    int ping_pong;
    int reverse;
    AnimationFrame *frames;
} Animation;

typedef struct Frame
{
    int duration;
    int tileid;
} Frame;

// Forward declaration of textureimage
typedef struct TextureImage
{
    char filename[MAX_FILENAME_LENGTH];
    uint32_t num_frames;
    Frame *frames;
    int current_frame;
    SDL_Texture *texture;
    struct TextureImage *next;
} TextureImage;

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
    SDL_Texture *texture;
} Tileset;

void tileset_load(App *app, Tileset *tileset, const char *filename);
void tileset_free(Tileset *tiles);
uint32_t tileset_get_animation_frame_id(Frame *frame);

#endif