

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
    Tileset * map_tiles = tileset_load(&app, "../assets/map_tiles.tsj");
    Tileset * player_tiles = tileset_load(&app, "../assets/player_tiles.tsj");
    
    player_init(&app, &player, player_tiles);
    map_init(&app, &map, map_tiles, "../assets/home.tmj");

    then = SDL_GetTicks();
    //Camera camera = make_camera(&app, SCREEN_WIDTH, SCREEN_HEIGHT);
    Camera camera = make_camera(&app, 1280, 720);
    app.camera = &camera;
    
    while (1) {
        draw_prepare_scene(&app, camera.target);
        input_handle(&app);
        player_handle(&app, &map, &camera, &player);
        map_draw(&app, &map);
        player_draw(&app, &player);
        // Screen
        draw_prepare_scene(&app, NULL);
        draw_camera_to_screen(&app, &camera);
        capFrameRate(&then, &remainder);
    }

    SDL_DestroyRenderer(app.renderer);
    SDL_DestroyWindow(app.window);
    player_free(&player);
    tileset_free(player_tiles);
    tileset_free(map_tiles);
    map_free(&map);
    IMG_Quit();
    SDL_Quit();

    return 0;
}