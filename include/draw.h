#ifndef DRAW_H
#define DRAW_H

#include "structs.h"

void draw_prepare_scene(App *app);
void draw_present_scene(App *app);
int draw_load_texture(App *app, struct TextureImage *image);
void draw_blit_texture(App *app, struct Entity *entity);

#endif