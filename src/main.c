

// Load sdl
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

// Load stdio
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "structs.h"
#include "draw.h"
#include "init.h"
#include "input.h"

// Logging
#include <log.h>

int main(int argc, char* argv[]) {
    App app;

    log_info("Starting up...");
    // Init SDL
    initSDL(&app);

    while (1) {
        doInput(&app);
        prepareScene(&app);
        presentScene(&app);
        SDL_Delay(16);
    }

    SDL_DestroyRenderer(app.renderer);
    SDL_DestroyWindow(app.window);

    SDL_Quit();

    return 0;
}