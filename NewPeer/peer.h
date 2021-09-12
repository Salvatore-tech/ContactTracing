//
// Created by saso on 03/09/2021.
//

#ifndef NEWPEER_PEER_H
#define NEWPEER_PEER_H

#include <stddef.h>
#include "header.h"

struct sockaddr_in enterIntoNetwork(int *fd_server);
void contactNeighbour(int fd_server);
void* listBroadcast();

void sendBroadcast();
void *sendId(void *args);
void *recvId();
void generate_id(size_t size, char* str);
#endif //NEWPEER_PEER_H
