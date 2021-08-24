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
void contactNeighbour();
void listBroadcast(int *brd_fd, const node_t *head_id, const node_t *tail_id);

void sendBroadcast(char**id);
void *sendId(void *args);
void *recvId();
void generate_id(size_t size, char* str);
#endif //UNTITLED1_PEER_H
