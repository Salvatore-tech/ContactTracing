//
// Created by saso on 08/08/21.
//

#include <unistd.h>
#include <resolv.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "server.h"

void handlePacket(int client_fd, struct sockaddr_in *client_addr) {
    msg_type_t msg;
    int port_to_bind;
    if (read(client_fd, (void *) &msg, sizeof(msg_type_t)) == -1) {
        perror("Read error\n");
        exit(-1);
    }
    switch (msg) {
        case MSG_UP:
            peerList[users_count++] = add_peer_node(client_addr);
            msg_type_t msg_to_send = CHK_MSG_UP;


            if (write(client_fd, &msg_to_send, sizeof(msg_to_send)) < 0) {
                perror("Failed to send ACK message\n");
                exit(-1);
            }
            break;

            // Delete user from the peerList in case of receiving MSG_DOWN
        case MSG_DOWN: {
            for (int i = 0; i < MAX_USERS; i++) {
                if (&peerList[i] && memcmp((void *) &peerList[i], (void *) client_addr, sizeof(client_addr)) == 0) {
                    // free(peerList[i]);
                    //peerList[i].peer_sock_addr;
                    users_count--;
                }
            }
        }
            break;
        case MSG_ALL: {
            msg_num_all_t msgNumAll;
            struct sockaddr_in* peerListToSend = malloc(users_count * sizeof (struct sockaddr_in));
            msgNumAll.msg = ACK_MSG_ALL;
            msgNumAll.noPeer = users_count;
            if (write(client_fd, &msgNumAll, sizeof(msgNumAll)) < 0) {
                perror("Failed to send ACK_MSG_ALL message\n");
                exit(-1);
            }

            getAllPeers(peerListToSend);
            int n = users_count * sizeof (struct sockaddr_in);
            if (write(client_fd, peerListToSend, n) < 0) {
                perror("Failed to send ACK_MSG_ALL message\n");
                exit(-1);
            }

            break;

            default: {
                printf("Error! The client has sent an uknown type of packet\n");
            }
            break;

        }
            //close(client_fd);
    }
}

void addPeer(msg_peer_t *msg) {

}

void getAllPeers(struct sockaddr_in *msg_to_send) {
    int n = users_count * sizeof (struct sockaddr_in);
    for(int i=0; i<users_count; i++)
        memcpy(&msg_to_send[i], &peerList[i]->peer_sock_addr, n);
}


msg_peer_t *add_peer_node(const struct sockaddr_in *new_peer_addr) {
    msg_peer_t *new_node = malloc(sizeof(msg_peer_t));
    new_node->peer_sock_addr = *new_peer_addr;
    return new_node;
}


int check_signal(unsigned int probability) {
    return ((rand() % 100) + 1 <= probability) ? 1 : 0;
}

void send_signal() {
    if (users_count == 0)
        exit(-1);
    int rnd = rand() % users_count + 1;
    int sockfd;
    // JACK:
    void *pck;
    struct sockaddr_in peer_addr;
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) {
        fprintf(stderr, "socket error\n");
        exit(1);
    }
    peer_addr.sin_family = AF_INET;
    peer_addr.sin_addr = peerList[rnd]->peer_sock_addr.sin_addr;
    peer_addr.sin_port = peerList[rnd]->peer_sock_addr.sin_port;
    if (sendto(sockfd, pck, sizeof(pck), 0, (struct sockaddr *) &peer_addr, sizeof(peer_addr)) < 0) {
        perror("Sendto error when sending alarm message\n");
        exit(-1);
    }
}