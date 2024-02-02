

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
#include "player.h"

// Logging
#include <log.h>

static void capFrameRate(long *then, float *remainder)
{
	long wait, frameTime;

	wait = 16 + *remainder;

	*remainder -= (int)*remainder;

	frameTime = SDL_GetTicks() - *then;

	wait -= frameTime;

	if (wait < 1)
	{
		wait = 1;
	}

	SDL_Delay(wait);

	*remainder += 0.667;

	*then = SDL_GetTicks();
}

int main(int argc, char* argv[]) {
    App app = {0};
    struct Entity player;
    long then;
	float remainder;

    log_info("Starting up...");
    // Init SDL
    init_sdl(&app);
    init_player(&app, &player);

    then = SDL_GetTicks();

	remainder = 0;
    
    while (1) {
        draw_prepare_scene(&app);
        input_handle(&app);
        handle_player(&app, &player);
        draw_blit_texture(&app, &player.texture, player.x, player.y);
        draw_present_scene(&app);
        capFrameRate(&then, &remainder);
    }

    SDL_DestroyRenderer(app.renderer);
    SDL_DestroyWindow(app.window);
    IMG_Quit();
    SDL_Quit();

    return 0;
}