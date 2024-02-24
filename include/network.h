#ifndef NETWORK_H
#define NETWORK_H

#include <stdint.h>

typedef enum { MOVE, SHOOT, HIT, KILL, CONNECT, DISCONNECT } NetPacketType;

typedef struct NetPacket {
  NetPacketType type;
  int facing;
  uint32_t id;
  int x;
  int y;

} NetPacket;

uint32_t network_init();
void network_destroy();
int network_service(NetPacket *p);
void network_send(NetPacket *p);

#endif
