#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "structs.h"
#include "draw.h"

// Logging
#include <log.h>

#define SPRITE_NUM_FRAMES 4
int player_init(App * app, struct Entity * player) {
    player->x = 100;
    player->y = 100;
    strcpy(player->texture.filename, "../assets/Zuul.png");
    player->width = SPRITE_WIDTH;
    player->height = SPRITE_HEIGHT;
    player->texture.frames = malloc(sizeof(SDL_Rect) * SPRITE_NUM_FRAMES);
    for(int i = 0; i < SPRITE_NUM_FRAMES; i++) {
        player->texture.frames[i].x = 0;
        player->texture.frames[i].y = i * SPRITE_WIDTH;
        player->texture.frames[i].w = SPRITE_WIDTH;
        player->texture.frames[i].h = SPRITE_HEIGHT;
    }

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
    }
    if (app->keyboard[SDL_SCANCODE_DOWN]) {
        player->y += PLAYER_SPEED;
    }
    if (app->keyboard[SDL_SCANCODE_LEFT]) {
        player->x -= PLAYER_SPEED;
    }
    if (app->keyboard[SDL_SCANCODE_RIGHT]) {
        player->x += PLAYER_SPEED;
    }
    player->x += player->dx;
    player->y += player->dy;
}

SDL_Rect * player_get_current_frame(struct Entity * player) {
    SDL_Rect * frame = &player->texture.frames[player->texture.current_frame];
    player->texture.current_frame = (player->texture.current_frame + 1) % SPRITE_NUM_FRAMES;
    return frame;
}

void player_Quit(struct Entity * player) {
    free(player->texture.frames);
}