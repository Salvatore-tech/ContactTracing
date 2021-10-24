//
// Created by saso on 03/09/2021.
//

#ifndef NEWPEER_PEER_H
#define NEWPEER_PEER_H

#include <stddef.h>

static char const delimiter = '_'; //TODO: edited

struct sockaddr_in enterIntoNetwork(int *fd_server, struct sockaddr_in *checkPosAddr);

void contactNeighbour(int fd_server, const struct sockaddr_in *myAddr);

void sendId(const struct sockaddr_in *peerToContact);

_Noreturn void *recvId(void *args);

_Noreturn void *sendBroadcast(void *args);

void *listBroadcast();

void generate_id(size_t size, char *str);

ssize_t fullWrite(int fd, const void *buf, size_t count);

ssize_t fullRead(int fd, void *buf, size_t count);

int buildSocket(int so_sobroadcast, const struct sockaddr_in *addr);

#endif //NEWPEER_PEER_H
