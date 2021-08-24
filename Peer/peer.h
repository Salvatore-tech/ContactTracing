//
// Created by saso on 07/08/21.
//

#ifndef UNTITLED1_PEER_H
#define UNTITLED1_PEER_H
#include <stddef.h>
#include <netinet/in.h>
#include <pthread.h>
#include "header.h"
#include "ds_peer.h"


int enterIntoNetwork(int *fd_server, int *fd_listener);
void requestNeighbour(int fd_server, struct sockaddr_in* neigh_addr);
char* contactNeighbour(int fd_neighbour, const struct sockaddr_in* neigh_addr, char* my_id, node_t** tail_id);
void listBroadcast(int *brd_fd, const node_t *head_id, const node_t *tail_id);

void sendBroadcast(char**id);
void readIncomingMessage(int fd_listener, node_t** tail_id);
void generate_id(size_t size, char* str);
#endif //UNTITLED1_PEER_H
