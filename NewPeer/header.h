//
// Created by saso on 03/09/2021.
//

#ifndef NEWPEER_HEADER_H
#define NEWPEER_HEADER_H

#include <netinet/in.h>

#define MSG_UP 2
#define CHK_MSG_UP 3
#define MSG_DOWN 4
#define MSG_ALL 5
#define ACK_MSG_ALL 6
#define MSG_BRD 7
#define MSG_GET_NEIGH 8
#define MSG_CONN 9
#define ACK_CONN 10

#define SRV_PORT 8500
#define BROAD_PORT 9000
#define NEIGH_NO 2

#define ID_LEN 66

typedef int msg_type_t;

typedef struct node{
    char *id;
    char *neigh_ids;
    struct node *next;
} node_t;

node_t* head;
node_t* tail;


typedef struct {
    msg_type_t msg;
} msg_ack_t;


typedef struct
{
    msg_type_t msg;           // = MSG_PEER
    struct sockaddr_in peer_sock_addr;
} msg_peer_t;

typedef struct {
    msg_type_t msg;
    char id[ID_LEN];
}msg_neigh_id_t;

typedef struct
{
    msg_type_t msg;
    char *ids; // Array of ids
} msg_brd_t;

typedef struct {
    msg_type_t msg;
    int noPeer;
}msg_num_all_t;
#endif //NEWPEER_HEADER_H
