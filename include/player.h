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

int player_init(App *app, Tileset *tileset);
void player_handle(App *app, Map *map, Camera * camera);
void player_draw(App *app);
void player_free();
void player_move(Map *map);
struct Entity * player_get();
#endif