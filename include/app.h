#ifndef APP_H
#define APP_H

#include <SDL2/SDL.h>
#include "defs.h"

typedef struct
{
    SDL_Renderer *renderer;
    SDL_Texture *target;

    int width;
    int height;

    int target_width;
    int target_height;

    int x, y;
} Camera;
typedef struct
{
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Surface *surface;
    SDL_Event event;
    Camera *camera;
    int keyboard[MAX_KEYBOARD_KEYS];
    int key_pressed;
    int num_keys_pressed;
    const char *assets_path;
} App;

#endif