

// Load sdl
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

// Load stdio
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "network.h"

#include "structs.h"
#include "draw.h"
#include "init.h"
#include "input.h"
#include "player.h"
#include "map.h"
#include "tileset.h"
#include "assets.h"

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
    long then;
	float remainder = 0;

    log_info("Starting up...");
    // Init SDL
    init_sdl(&app);
    // Init tilesets
    asset_init();

    network_init();

    Tileset * map_tiles = tileset_load(&app, asset_path("map_tiles.tsj"));
    Tileset * player_tiles = tileset_load(&app, asset_path("player_tiles.tsj"));
    Camera camera = make_camera(&app, 1280, 720);
    app.camera = &camera;
    
    player_init(&app, player_tiles);
    map_init(&map, map_tiles, asset_path("home.tmj"));

    then = SDL_GetTicks();
    //Camera camera = make_camera(&app, SCREEN_WIDTH, SCREEN_HEIGHT);
    NetPacket send_p;
    NetPacket recv_p;

    // Send create player packet
    send_p.type = CONNECT;
    network_send(&send_p);
    struct Entity* player_list = NULL;
    int player_list_size = 0;

    while (1) {
        network_service(&recv_p);
        if (recv_p.type == CONNECT) {
            player_list = realloc(player_list, sizeof(struct Entity) * (player_list_size + 1));
            player_list_size++;
            log_debug("Received connect packet");
        }
        if (recv_p.type == MOVE) {
            if (player_list_size > 0) {
                player_list[0].x = recv_p.x;
                player_list[0].y = recv_p.y;
                log_debug("Received move packet");
            }
        }
        draw_prepare_scene(&app, camera.target);
        input_handle(&app);
        player_handle(&app, &map, &camera);
        struct Entity * e = player_get();
        send_p.type = MOVE;
        send_p.x = e->x;
        send_p.y = e->y;
        network_send(&send_p);
        map_draw(&app, &map);
        player_draw(&app);
        if (player_list_size > 0) {
            for (int i = 0; i < player_list_size; i++) {
                entity_draw(&app, player_tiles, &player_list[i]);
            }
        }
        camera_update(&camera, player_get(), map.width*map.tilewidth, map.height*map.tileheight);
        // Screen
        draw_prepare_scene(&app, NULL);
        draw_camera_to_screen(&app, &camera);
        capFrameRate(&then, &remainder);
    }

    SDL_DestroyRenderer(app.renderer);
    SDL_DestroyWindow(app.window);
    player_free();
    tileset_free(player_tiles);
    tileset_free(map_tiles);
    map_free(&map);
    asset_free();
    IMG_Quit();
    SDL_Quit();
    network_destroy();

    return 0;
}