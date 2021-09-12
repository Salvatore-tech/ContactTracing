//
// Created by saso on 03/09/2021.
//

#ifndef NEWSERVER_SERVER_H
#define NEWSERVER_SERVER_H

#include <netinet/in.h>

#define MSG_UP 2
#define CHK_MSG_UP 3
#define MSG_DOWN 4
#define MSG_ALL 5
#define ACK_MSG_ALL 6

#define MSG_ERR 6
#define MSG_BRD 7
#define MSG_GET_NEIGH 8
#define MSG_ALARM 9
#define LENGHT_NAME 30
#define MAX_USERS 50
#define SRV_PORT 8500

typedef int msg_type_t;
static const char *ip_str = "127.0.0.1";


typedef struct
{
    msg_type_t msg; // = MSG_PEER
    struct sockaddr_in peer_sock_addr;
} msg_peer_t;

typedef struct {
    msg_type_t msg;
    int noPeer;
}msg_num_all_t;


static unsigned int users_count = 0;
extern struct sockaddr_in peerList[MAX_USERS];

int handlePacket(int client_fd);


struct sockaddr_in* add_peer_node(const struct sockaddr_in* new_peer_addr);
int check_signal(unsigned int probability);
void send_signal();
void getAllPeers(struct sockaddr_in* msg_to_send);

#endif //NEWSERVER_SERVER_H