#ifndef PLAYER_H
#define PLAYER_H
#include "structs.h"

int init_player(App *app, struct Entity *player);
void handle_player(App *app, struct Entity *player);

#endif