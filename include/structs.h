#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "defs.h"
#include "input.h"
#include "tileset.h"

#ifndef ENTITY_H
#define ENTITY_H

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
    Tileset *tileset;
    struct Entity *next;
};

#endif