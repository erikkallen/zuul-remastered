#ifndef PLAYER_H
#define PLAYER_H
#include "app.h"
#include "structs.h"
#include "tileset.h"

#define PLAYER_FACING_LEFT 0
#define PLAYER_FACING_RIGHT 1
#define PLAYER_FACING_UP 2
#define PLAYER_FACING_DOWN 3

int player_init(App *app, struct Entity *player, Tileset *tileset);
void player_handle(App *app, struct Entity *player);
void player_draw(App *app, struct Entity *player);
void player_free(struct Entity *player);

#endif