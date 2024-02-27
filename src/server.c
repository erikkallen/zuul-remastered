#define ENET_IMPLEMENTATION
#include <enet.h>
#include <stdio.h>

#include "network.h"

#include "log.h"

#define MAX_CLIENTS 32

typedef struct {
  uint32_t id;
  bool connected;
  float x;
  float y;
  uint8_t facing;
} Player;

// Player list
Player *players = NULL;
int player_count = 0;

void broadcastToOthers(ENetHost *server, ENetEvent *event) {
  for (int i = 0; i < server->peerCount; ++i) {
    ENetPeer *peer = &server->peers[i];
    if (peer->connectID == event->peer->connectID) {
      continue;
    }

    // Check if the peer is connected and is not the sender
    if (peer->state == ENET_PEER_STATE_CONNECTED && peer != event->peer) {
      // Create a copy of the packet for each client you are sending to
      ENetPacket *outgoingPacket =
          enet_packet_create(event->packet->data, event->packet->dataLength,
                             ENET_PACKET_FLAG_RELIABLE);

      // Send the packet to the peer
      enet_peer_send(peer, 0, outgoingPacket);
    }
  }

  // Don't forget to cleanup the received packet
  // enet_packet_destroy(event->packet);
}

void broadcast_player_list(ENetHost *server, ENetEvent *e) {
  // Check if the player is already in the list
  bool found = false;
  NetPacket *recv_p = (NetPacket *)e->packet->data;
  if (recv_p == NULL) {
    log_error("Packet is NULL");
    return;
  }

  for (int i = 0; i < player_count; i++) {
    if (players[i].id == recv_p->id) {
      found = true;
    }
  }

  if (!found && player_count < MAX_CLIENTS) {
    players = realloc(players, sizeof(Player) * (player_count + 1));
    players[player_count].id = recv_p->id;
    players[player_count].connected = true;
    players[player_count].x = 0;
    players[player_count].y = 0;
    players[player_count].facing = 0;
    player_count++;
  }

  // Send connect packet
  for (int i = 0; i < player_count; ++i) {
    if (players[i].connected == false) {
      continue;
    }
    Player *player = &players[i];
    NetPacket p;
    p.type = CONNECT;
    p.id = player->id;
    p.x = 0;
    p.y = 0;
    log_debug("Sending connect %u packet to %u", player->id, recv_p->id);
    // Handle connect packet
    // broadcastToOthers(server, &event);
    ENetPacket *packet =
        enet_packet_create(&p, sizeof(NetPacket), ENET_PACKET_FLAG_RELIABLE);
    enet_host_broadcast(server, 0, packet);
  }
}

void handle_receive(ENetHost *server, ENetEvent *e) {
  if (e->packet == NULL) {
    log_error("Packet is NULL");
    return;
  }
  NetPacket *p = (NetPacket *)e->packet->data;
  if (p == NULL) {
    log_error("Packet is NULL");
    enet_packet_destroy(e->packet);
    return;
  }
  switch (p->type) {
  case CONNECT: {
    broadcast_player_list(server, e);
    break;
  }
  case MOVE: {
    // Handle move packet
    broadcastToOthers(server, e);
    break;
  }
  default:
    break;
  }
}

int main() {
  if (enet_initialize() != 0) {
    printf("An error occurred while initializing ENet.\n");
    return 1;
  }

  ENetAddress address = {0};

  address.host = ENET_HOST_ANY; /* Bind the server to the default localhost. */
  address.port = 7777;          /* Bind the server to port 7777. */

  /* create a server */
  ENetHost *server = enet_host_create(&address, MAX_CLIENTS, 2, 0, 0);

  if (server == NULL) {
    printf("An error occurred while trying to create an ENet server host.\n");
    return 1;
  }

  printf("Started a server...\n");

  ENetEvent event;
  while (true) {
    /* Wait up to 1000 milliseconds for an event. (WARNING: blocking) */
    if (enet_host_service(server, &event, 100) > 0) {
      switch (event.type) {
      case ENET_EVENT_TYPE_CONNECT:
        handle_receive(server, &event);
        break;

      case ENET_EVENT_TYPE_RECEIVE: {
        handle_receive(server, &event);
        enet_packet_destroy(event.packet);
        break;
      }
      case ENET_EVENT_TYPE_DISCONNECT:
        printf("%s disconnected.\n", (char *)event.peer->data);
        /* Reset the peer's client information. */
        event.peer->data = NULL;
        uint32_t player_count_temp = player_count;
        for (int i = 0; i < player_count_temp; i++) {
          if (players[i].id == event.peer->connectID) {
            players[i].connected = false;
            player_count--;
          }
        }
        // remove_client(client_list, event.peer->connectID);
        break;

      case ENET_EVENT_TYPE_DISCONNECT_TIMEOUT:
        printf("%u disconnected due to timeout.\n", event.peer->connectID);
        /* Reset the peer's client information. */
        // remove_client(client_list, event.peer->connectID);
        event.peer->data = NULL;
        break;

      case ENET_EVENT_TYPE_NONE:
        break;
      }
    }
  }

  enet_host_destroy(server);
  enet_deinitialize();
  // free_clients(client_list);
  return 0;
}
