#ifndef PLAYER_H
#define PLAYER_H
#include "app.h"
#include "structs.h"
#include "tileset.h"
#include "map.h"
#include "draw.h"

#define PLAYER_FACING_LEFT 6
#define PLAYER_FACING_RIGHT 9
#define PLAYER_FACING_UP 3
#define PLAYER_FACING_DOWN 0

int player_init(App *app, struct Entity *player, Tileset *tileset);
void player_handle(App *app, Map *map, Camera * camera, struct Entity *player);
void player_draw(App *app, struct Entity *player);
void player_free(struct Entity *player);

#endif