

#define ENET_IMPLEMENTATION
#include <enet.h>
#include <stdio.h>

#include "network.h"

#include <log.h>

ENetHost *client = {0};
ENetEvent event = {0};
ENetPeer *peer = {0};

uint32_t net_id = 0;

uint32_t network_init() {
  if (enet_initialize() != 0) {
    fprintf(stderr, "An error occurred while initializing ENet.\n");
    return 0;
  }

  client = enet_host_create(NULL /* create a client host */,
                            1 /* only allow 1 outgoing connection */,
                            2 /* allow up 2 channels to be used, 0 and 1 */,
                            0 /* assume any amount of incoming bandwidth */,
                            0 /* assume any amount of outgoing bandwidth */);
  if (client == NULL) {
    fprintf(stderr,
            "An error occurred while trying to create an ENet client host.\n");
    exit(EXIT_FAILURE);
  }

  ENetAddress address = {0};
  /* Connect to some.server.net:1234. */
  enet_address_set_host(&address, "127.0.0.1");
  address.port = 7777;
  /* Initiate the connection, allocating the two channels 0 and 1. */
  peer = enet_host_connect(client, &address, 2, 0);
  if (peer == NULL) {
    log_error("No available peers for initiating an ENet connection.\n");
    exit(EXIT_FAILURE);
  }

  return 0;
}

int network_player_connect(uint32_t player_id) {
  // Generate player id
  log_info("Sending connect %u packet", player_id);
  NetPacket connect_p;
  connect_p.type = CONNECT;
  connect_p.id = player_id;
  network_send(&connect_p);
  return 0;
}

NetPacket move_p;
int network_send_player_pos(struct Entity *player) {
  move_p.type = MOVE;
  move_p.id = player->id;
  move_p.x = player->x;
  move_p.y = player->y;
  move_p.facing = player->facing;
  network_send(&move_p);
  return 0;
}

int network_service(NetPacket *cp, NetState *state) {
  if (enet_host_service(client, &event, 0) > 0) {
    switch (event.type) {
    case ENET_EVENT_TYPE_CONNECT:
      log_info("Connection  to host succeeded. %d", event.peer->connectID);
      cp->type = HOST_CONNECT;
      cp->id = event.peer->connectID;
      return 0;
      break;
    case ENET_EVENT_TYPE_RECEIVE: {
      if (event.packet == NULL) {
        log_error("Packet is NULL");
        return -1;
      }
      NetPacket *p = (NetPacket *)event.packet->data;
      if (p == NULL) {
        log_error("Packet is NULL");
        enet_packet_destroy(event.packet);
        return -1;
      }
      cp->x = p->x;
      cp->y = p->y;
      cp->id = p->id;
      cp->type = p->type;

      log_debug("Packet received: %d %d %d %u", p->type, p->x, p->y, p->id);
      enet_packet_destroy(event.packet);
      return 0;
    } break;
    case ENET_EVENT_TYPE_DISCONNECT:
      log_info("Disconnection succeeded.");
      // disconnected = true;
      event.peer->data = NULL;
      break;
    default:
      enet_packet_destroy(event.packet);

      log_debug("Unhandled event");
    }
  }
  return -1;
}

void network_send(NetPacket *p) {
  /* Create a reliable packet of size 7 containing "packet\0" */
  ENetPacket *packet =
      enet_packet_create(p, sizeof(NetPacket), ENET_PACKET_FLAG_RELIABLE);
  /* Extend the packet so and append the string "foo", so it now */
  /* contains "packetfoo\0"                                      */
  // enet_packet_resize (packet, strlen ("packetfoo") + 1);
  // strcpy (& packet -> data [strlen ("packet")], "foo");
  /* Send the packet to the peer over channel id 0. */
  /* One could also broadcast the packet by         */
  // enet_host_broadcast (client, 0, packet);
  enet_peer_send(peer, 0, packet);
}

void network_destroy() {
  enet_peer_disconnect_now(peer, 0);
  enet_host_destroy(client);
  enet_deinitialize();
}
