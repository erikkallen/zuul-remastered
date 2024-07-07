#include "defs.h"
#include "map.h"
#include "tileset.h"
#include <SDL3/SDL.h>
#include <SDL3/SDL_image.h>

#ifndef ENTITY_H
#define ENTITY_H

struct Entity {
  int id;
  int x;
  int y;
  int x_abs;
  int y_abs;
  int width;
  int height;
  float dx;
  float dy;
  int health;
  int facing;
  int move_speed;
  Tileset *tileset;
  struct Entity *next;
  const char *map_name;
};

#endif
