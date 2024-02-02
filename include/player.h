#ifndef PLAYER_H
#define PLAYER_H
#include "structs.h"

int player_init(App *app, struct Entity *player);
void player_handle(App *app, struct Entity *player);
SDL_Rect *player_get_current_frame(struct Entity *player);
void player_Quit(struct Entity *player);

#endif