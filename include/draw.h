#ifndef DRAW_H
#define DRAW_H

#include "structs.h"
#include "app.h"

#define CAMERA_BORDER 1

Camera make_camera(App *app, int width, int height);
void draw_prepare_scene(App *app, SDL_Texture *target);
int draw_load_texture(App *app, struct TextureImage *image, const char *filename);
void draw_camera_to_screen(App *app, Camera *camera);
#endif