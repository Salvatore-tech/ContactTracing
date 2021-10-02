//
// Created by saso on 03/09/2021.
//

#ifndef NEWPEER_COMMON_H
#define NEWPEER_COMMON_H

#include <netinet/in.h>

#define MSG_UP 1
#define ACK_MSG_UP 2
#define MSG_ALL 3
#define ACK_MSG_ALL 4
#define MSG_BRD 5
#define MSG_CONN 6
#define ACK_CONN 7
#define MSG_POS 8

#define SRV_PORT 8500
#define BROAD_PORT 9000

#define ID_LEN 66

typedef int msg_type_t;

typedef struct node {
    int no_cont;
    char id[ID_LEN + 1];
    char *neigh_ids;
    struct node *next;
} node_t;

extern node_t *head;
extern node_t *tail;

typedef struct {
    msg_type_t msg;
    struct sockaddr_in peer_sock_addr;
    struct sockaddr_in check_pos_addr;
} msg_peer_t;

typedef struct {
    msg_type_t msg;
    char id[ID_LEN];
} msg_neigh_id_t;

typedef struct {
    msg_type_t msg;
    int noPeer;
} msg_num_all_t;
#endif //NEWPEER_COMMON_H
