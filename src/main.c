// Load sdl
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

// Load stdio
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "network.h"

#include "assets.h"
#include "draw.h"
#include "init.h"
#include "input.h"
#include "map.h"
#include "player.h"
#include "structs.h"
#include "tileset.h"

// Logging
#include <log.h>

static void capFrameRate(uint32_t *then, float *remainder) {
  uint32_t wait, frameTime;

  wait = 16 + *remainder;

  *remainder -= (int)*remainder;

  frameTime = SDL_GetTicks() - *then;

  wait -= frameTime;

  if (wait < 1) {
    wait = 1;
  }

  SDL_Delay(wait);

  *remainder += 0.667;

  *then = SDL_GetTicks();
}

// Main function
int main(int argc, char *argv[]) {
  App app = {0};

  Map map = {0};
  uint32_t then;
  float remainder = 0;

  log_info("Starting up...");
  // Init SDL
  init_sdl(&app);
  // Init tilesets
  asset_init();

  uint32_t net_id = network_init();

  Tileset *map_tiles = tileset_load(&app, asset_path("map_tiles.tsj"));
  Tileset *player_tiles = tileset_load(&app, asset_path("player_tiles.tsj"));
  Camera camera = make_camera(&app, 1280, 720);
  app.camera = &camera;

  player_init(&app, player_tiles);
  map_init(&map, map_tiles, asset_path("home.tmj"));

  then = SDL_GetTicks();
  NetPacket send_p;
  NetPacket recv_p;

  // Send create player packet
  send_p.type = CONNECT;
  network_send(&send_p);
  struct Entity *player_list = NULL;
  int player_list_size = 0;

  while (1) {
    if (network_service(&recv_p) == 0) {
      if (recv_p.type == CONNECT) {
        bool found = false;
        for (int i = 0; i < player_list_size; i++) {
          if (recv_p.id == player_list[i].id && recv_p.id != net_id) {
            found = true;
          }
        }
        if (!found) {
          player_list = realloc(player_list,
                                sizeof(struct Entity) * (player_list_size + 1));
          player_list[player_list_size].id = recv_p.id;
          player_list_size++;
          log_debug("Received connect packet %u", recv_p.id);
        }
      }
      if (recv_p.type == MOVE) {
        if (player_list_size > 0) {
          for (int i = 0; i < player_list_size; i++) {
            if (player_list[i].id == recv_p.id) {
              player_list[i].x = recv_p.x;
              player_list[i].y = recv_p.y;
              player_list[i].facing = recv_p.facing;
              // log_debug("Received move packet");
            }
          }
        }
      }
    }
    draw_prepare_scene(&app, camera.target);
    input_handle(&app);
    player_handle(&app, &map, &camera);
    struct Entity *e = player_get();
    send_p.type = MOVE;
    send_p.x = e->x;
    send_p.y = e->y;
    send_p.facing = e->facing;
    network_send(&send_p);
    map_draw(&app, &map);
    player_draw(&app);
    if (player_list_size > 0) {
      for (int i = 0; i < player_list_size; i++) {
        entity_draw(&app, player_tiles, &player_list[i]);
      }
    }
    camera_update(&camera, player_get(), map.width * map.tilewidth,
                  map.height * map.tileheight);
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
