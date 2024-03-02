#ifndef NETWORK_H
#define NETWORK_H

#include "player.h"
#include <stdint.h>

typedef enum {
  ID,
  MOVE,
  SHOOT,
  HIT,
  KILL,
  CONNECT,
  DISCONNECT,
  HOST_CONNECT
} NetPacketType;

typedef struct NetPacket {
  NetPacketType type;
  int facing;
  uint32_t id;
  int x;
  int y;
  int move_speed;

} NetPacket;

typedef struct NetState {
  uint32_t id;
  int connected;
} NetState;

uint32_t network_init(char *host, uint16_t port);
void network_destroy();
int network_service(NetPacket *p, NetState *s);
void network_send(NetPacket *p);
int network_player_connect(uint32_t player_id);
int network_send_player_pos(struct Entity *player);

#endif
