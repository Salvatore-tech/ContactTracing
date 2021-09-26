//
// Created by saso on 03/09/2021.
//

#ifndef NEWSERVER_SERVER_H
#define NEWSERVER_SERVER_H

#include <netinet/in.h>
#include <poll.h>


#define MSG_UP 2
#define ACK_MSG_UP 3
#define MSG_ALL 5
#define ACK_MSG_ALL 6

#define MSG_POS 11

#define MAX_USERS 50
#define SRV_PORT 8500

#define TIMEOUT 500

typedef int msg_type_t;
static unsigned int users_count = 0;
static const unsigned int periodPositive = 60;

static struct sockaddr_in peerList[MAX_USERS];
static struct sockaddr_in positivityAddr[MAX_USERS];
extern struct pollfd client_poll_struct[MAX_USERS];

typedef struct {
    msg_type_t msg; // = MSG_PEER
    struct sockaddr_in peerAddr;
    struct sockaddr_in positivityAddr;
} msg_peer_t;

typedef struct {
    msg_type_t msg;
    unsigned int noPeer;
} msg_num_all_t;

int buildSocket(int enable_reuse, struct sockaddr_in *servSockaddr);

void initPollStruct(int list_sd);

int launchDetachThreadToSignalPositivePeer();

int handlePacket(int client_fd);

ssize_t fullWrite(int fd, const void *buf, size_t count);

void *signalPositive();

#endif //NEWSERVER_SERVER_H
