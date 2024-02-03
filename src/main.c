

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
#include "map.h"
#include "tileset.h"

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
    Tileset map_tiles = {0};
    Tileset player_tiles = {0};
    App app = {0};
    Map map = {0};
    struct Entity player;
    long then;
	float remainder = 0;

    // Read assets path string from command line
    if (argc < 2) {
        log_error("Usage: %s <assets path>", argv[0]);
        exit(1);
    }
    app.assets_path = argv[1];

    log_info("Starting up...");
    // Init SDL
    init_sdl(&app);
    // Init tilesets
    tileset_load(&app, &map_tiles, "../assets/map_tiles.tsj");
    tileset_load(&app, &player_tiles, "../assets/player_tiles.tsj");
    player_init(&app, &player, &player_tiles);
    map_init(&app, &map, &map_tiles, "../assets/home.tmj");

    then = SDL_GetTicks();
    
    while (1) {
        draw_prepare_scene(&app);
        map_draw(&app, &map);
        input_handle(&app);
        player_handle(&app, &player);
        player_draw(&app, &player);
        draw_present_scene(&app);
        capFrameRate(&then, &remainder);
    }

    SDL_DestroyRenderer(app.renderer);
    SDL_DestroyWindow(app.window);
    player_free(&player);
    tileset_free(&player_tiles);
    tileset_free(&map_tiles);
    map_free(&map);
    IMG_Quit();
    SDL_Quit();

    return 0;
}