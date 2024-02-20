#ifndef NETWORK_H
#define NETWORK_H

typedef enum {
        MOVE,
        SHOOT,
        HIT,
        KILL,
        CONNECT,
        DISCONNECT
    } NetPacketType;

typedef struct NetPacket {
    NetPacketType type;
    int x;
    int y;

} NetPacket;

void network_init();
void network_destroy();
void network_service(NetPacket *p);
void network_send(NetPacket *p);

#endif