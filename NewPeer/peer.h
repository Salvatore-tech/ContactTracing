//
// Created by saso on 03/09/2021.
//

#ifndef NEWPEER_PEER_H
#define NEWPEER_PEER_H

#include <stddef.h>
#include "header.h"

static char *const delimiter = "_";

int buildSocket(int so_sobroadcast, const struct sockaddr_in* addr);

ssize_t fullWrite(int fd, const void *buf, size_t count);

ssize_t fullRead(int fd, void *buf, size_t count);

struct sockaddr_in enterIntoNetwork(int *fd_server, struct sockaddr_in *check_pos_addr);

void contactNeighbour(int fd_server, const struct sockaddr_in *myAddr);

void *listBroadcast();

void *sendBroadcast(void *args);

void sendId(const struct sockaddr_in* peerToContact);

void *recvId(void *args);

void generate_id(size_t size, char *str);

#endif //NEWPEER_PEER_H
