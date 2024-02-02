#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "structs.h"
#include "draw.h"

// Logging
#include <log.h>


int init_player(App * app, struct Entity * player) {
    player->x = 100;
    player->y = 100;
    strcpy(player->texture.filename, "../assets/Zuul-front-left.png");

    int rc = draw_load_texture(app, &player->texture);
    if (rc != 0) {
        log_error("Failed to load texture");
        exit(1);
    }
    SDL_QueryTexture(player->texture.texture, NULL, NULL, &player->width, &player->height);
    return 0;
}

void handle_player(App * app, struct Entity * player) {
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
