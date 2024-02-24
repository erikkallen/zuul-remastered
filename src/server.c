#define ENET_IMPLEMENTATION
#include <enet.h>
#include <stdio.h>

#include "network.h"

#include <log.h>

// typedef struct client_node {
//     uint32_t connectID; // You can add more client-related information here.
//     struct client_node *next;
// } client_node;

// client_node * client_list = NULL;

// client_node* add_client(client_node *head, uint32_t connectID) {
//     client_node *new_node = (client_node*)malloc(sizeof(client_node));
//     if (new_node == NULL) {
//         perror("Failed to allocate new node");
//         exit(EXIT_FAILURE);
//     }

//     new_node->connectID = connectID;
//     new_node->next = head; // Insert at the head for simplicity
//     head = new_node;

//     return head;
// }

// client_node* remove_client(client_node *head, int client_socket) {
//     client_node *current = head;
//     client_node *prev = NULL;

//     while (current != NULL) {
//         if (current->connectID == client_socket) {
//             if (prev == NULL) {
//                 head = current->next;
//             } else {
//                 prev->next = current->next;
//             }
//             free(current);
//             break;
//         }
//         prev = current;
//         current = current->next;
//     }

//     return head;
// }

// void free_clients(client_node * client_list) {
//         while (client_list != NULL) {
//         client_node *temp = client_list;
//         client_list = client_list->next;
//         free(temp);
//     }
// }

void broadcastToOthers(ENetHost *server, ENetEvent *event) {
  for (int i = 0; i < server->peerCount; ++i) {
    ENetPeer *peer = &server->peers[i];

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

int handle_add(ENetHost *server, ENetEvent *e) {
  printf("A new client %u connected.\n", e->peer->connectID);
  /* Store any relevant client information here. */
  // Send connect packet of online players
  log_info("Total players: %d", server->peerCount);
  for (int i = 0; i < server->peerCount; ++i) {
    ENetPeer *peer = &server->peers[i];
    if (peer->state == ENET_PEER_STATE_CONNECTED) {
      NetPacket p;
      p.type = CONNECT;
      p.id = peer->connectID;
      p.x = 0;
      p.y = 0;
      log_debug("Sending connect %u packet to %u", peer->connectID,
                e->peer->connectID);
      ENetPacket *packet =
          enet_packet_create(&p, sizeof(NetPacket), ENET_PACKET_FLAG_RELIABLE);
      enet_peer_send(e->peer, 0, packet);
      // enet_packet_destroy (packet);
    }
  }
  return 0;
}

int main() {
  if (enet_initialize() != 0) {
    printf("An error occurred while initializing ENet.\n");
    return 1;
  }

  ENetAddress address = {0};

  address.host = ENET_HOST_ANY; /* Bind the server to the default localhost. */
  address.port = 7777;          /* Bind the server to port 7777. */

#define MAX_CLIENTS 32

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
    while (enet_host_service(server, &event, 100) > 0) {
      switch (event.type) {
      case ENET_EVENT_TYPE_CONNECT:
        handle_add(server, &event);
        break;

      case ENET_EVENT_TYPE_RECEIVE: {
        // printf("A packet of length %lu containing %s was received from %s on
        // channel %u.\n",
        //         event.packet->dataLength,
        //         (char *)event.packet->data,
        //         (char *)event.peer->data,
        //         event.channelID);
        NetPacket *p = (NetPacket *)event.packet->data;
        switch (p->type) {
        case CONNECT:
          // Handle connect packet
          broadcastToOthers(server, &event);
          break;
        case MOVE:
          // Handle move packet
          broadcastToOthers(server, &event);
          break;

        default:
          break;
        }

        // log_debug("Player at %d %d", p->x, p->y);
        /* Clean up the packet now that we're done using it. */
        enet_packet_destroy(event.packet);
        break;
      }
      case ENET_EVENT_TYPE_DISCONNECT:
        printf("%s disconnected.\n", (char *)event.peer->data);
        /* Reset the peer's client information. */
        event.peer->data = NULL;
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
