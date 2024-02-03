#ifndef TILESET_H
#define TILESET_H

#include <stdint.h>
#include <SDL2/SDL.h>

#include "defs.h"
#include "app.h"

// Bits on the far end of the 32-bit global tile ID are used for tile flags
#define FLIPPED_HORIZONTALLY_FLAG 0x80000000
#define FLIPPED_VERTICALLY_FLAG 0x40000000
#define FLIPPED_DIAGONALLY_FLAG 0x20000000
#define ROTATED_HEXAGONAL_120_FLAG 0x10000000
#define TILE_FLAG_MASK 0xf0000000
#define TILE_ID_MASK 0x0fffffff

typedef struct AnimationFrame AnimationFrame;
typedef struct AnimationFrame
{
    int tileid;
    int duration;
    AnimationFrame *next;
} AnimationFrame;

typedef struct Animation Animation;
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

typedef struct Frame Frame;
typedef struct Frame
{
    SDL_Rect frame;
    Animation *animation;
    Frame *next;
} Frame;

// Forward declaration of textureimage
typedef struct TextureImage TextureImage;
typedef struct TextureImage
{
    char filename[MAX_FILENAME_LENGTH];
    uint32_t num_frames;
    Frame *frames;
    int current_frame;
    SDL_Texture *texture;
    TextureImage *next;
} TextureImage;

typedef struct Tileset Tileset;
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
    TextureImage tile_image;
    Tileset *next;
} Tileset;

void tileset_load(App *app, Tileset *tileset, const char *filename);
void tileset_free(Tileset *tiles);
uint32_t tileset_get_animation_frame_id(Frame *frame);

#endif