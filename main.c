

// Load sdl
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

// Load stdio
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Logging
#include <log.h>

int init_sdl() {
    // Init SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return 1;
    }

    return 0;
}

int main(int argc, char* argv[]) {
    log_info("Starting up...");
    // Init SDL
    if (init_sdl()) {
        return 1;
    }

    return 0;
}