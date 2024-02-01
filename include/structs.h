#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "defs.h"

#ifndef ENTITY_H
#define ENTITY_H

typedef struct
{
    int data[MAP_WIDTH][MAP_HEIGHT];
} Map;

struct AtlasImage
{
    char filename[MAX_FILENAME_LENGTH];
    SDL_Rect rect;
    SDL_Texture *texture;
    struct AtlasImage *next;
};

struct Entity
{
    int x;
    int y;
    int facing;
    struct AtlasImage *texture;
    struct Entity *next;
};

typedef struct
{
    SDL_Point renderOffset;
    struct Entity entityHead, *entityTail;
    Map map;
} Dungeon;

typedef struct
{
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Surface *surface;
    SDL_Event event;
} App;

#endif