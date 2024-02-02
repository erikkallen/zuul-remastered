#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "defs.h"
#include "input.h"

#ifndef ENTITY_H
#define ENTITY_H

struct TextureImage
{
    char filename[MAX_FILENAME_LENGTH];
    SDL_Rect *frames;
    int current_frame;
    SDL_Texture *texture;
    int animation_speed;
    int frame_offset;
    struct TextureImage *next;
};

struct Entity
{
    int x;
    int y;
    int width;
    int height;
    float dx;
    float dy;
    int health;
    int facing;
    int move_speed;
    struct TextureImage texture;
    struct Entity *next;
};

typedef struct
{
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Surface *surface;
    SDL_Event event;
    int keyboard[MAX_KEYBOARD_KEYS];
    int key_pressed;
} App;

#endif