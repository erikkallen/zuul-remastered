#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "structs.h"
#include "draw.h"

// Logging
#include <log.h>

#define SPRITE_NUM_FRAMES 8
#define ANIMATION_FRAMES 2
int player_init(App * app, struct Entity * player) {
    player->x = 100;
    player->y = 100;
    strcpy(player->texture.filename, "../assets/sprites.png");
    player->width = SPRITE_WIDTH;
    player->height = SPRITE_HEIGHT;
    player->texture.frames = malloc(sizeof(SDL_Rect) * SPRITE_NUM_FRAMES);
    for(int i = 0; i < SPRITE_NUM_FRAMES; i++) {
        player->texture.frames[i].x = 0;
        player->texture.frames[i].y = i * SPRITE_WIDTH;
        player->texture.frames[i].w = SPRITE_WIDTH;
        player->texture.frames[i].h = SPRITE_HEIGHT;
    }
    player->texture.animation_speed = 150;

    int rc = draw_load_texture(app, &player->texture);
    if (rc != 0) {
        log_error("Failed to load texture");
        exit(1);
    }
    //SDL_QueryTexture(player->texture.texture, NULL, NULL, &player->texture.rect.w, &player->texture.rect.h);
    return 0;
}

void player_handle(App * app, struct Entity * player) {
    if (app->keyboard[SDL_SCANCODE_UP]) {
        player->y -= PLAYER_SPEED;
        player->texture.frame_offset = 4;
    }
    if (app->keyboard[SDL_SCANCODE_DOWN]) {
        player->y += PLAYER_SPEED;
        player->texture.frame_offset = 6;
    }
    if (app->keyboard[SDL_SCANCODE_LEFT]) {
        player->x -= PLAYER_SPEED;
        player->texture.frame_offset = 0;
    }
    if (app->keyboard[SDL_SCANCODE_RIGHT]) {
        player->x += PLAYER_SPEED;
        player->texture.frame_offset = 2;
    }
    if (app->key_pressed == SDL_SCANCODE_UP || app->key_pressed == SDL_SCANCODE_DOWN || app->key_pressed == SDL_SCANCODE_LEFT || app->key_pressed == SDL_SCANCODE_RIGHT) {
        player->move_speed = PLAYER_SPEED;
    }
    else {
        player->move_speed = 0;
    }
    // player->x += player->dx;
    // player->y += player->dy;
}

SDL_Rect * player_get_current_frame(struct Entity * player) {
    SDL_Rect * frame = &player->texture.frames[player->texture.current_frame];
    int ticks = SDL_GetTicks();
    int sprite = (ticks / player->texture.animation_speed) % ANIMATION_FRAMES;
    if (player->move_speed == 0) {
        sprite = 0;
    }
    player->texture.current_frame = player->texture.frame_offset + sprite;//(player->texture.current_frame + 1) % SPRITE_NUM_FRAMES;
    return frame;
}

void player_free(struct Entity * player) {
    free(player->texture.frames);
    SDL_DestroyTexture(player->texture.texture);
}