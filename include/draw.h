#ifndef DRAW_H
#define DRAW_H

#include "structs.h"
#include "app.h"

#define CAMERA_BORDER 1

Camera make_camera(App *app, int width, int height);
void draw_prepare_scene(App *app, SDL_Texture *target);
void draw_camera_to_screen(App *app, Camera *camera);
void camera_update(Camera * camera, struct Entity * player, int map_width, int map_height);
void entity_draw(App* app, Tileset* tileset, struct Entity* entity);
#endif