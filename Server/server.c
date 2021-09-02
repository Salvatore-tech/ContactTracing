//
// Created by saso on 08/08/21.
//

#include <unistd.h>
#include <resolv.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include <arpa/inet.h>
#include "server.h"

int handlePacket(int client_fd, struct sockaddr_in **peerList, struct sockaddr_in *client_addr) {
    msg_type_t msg;
    msg_peer_t msg_to_send;
    int n;

    int port_to_bind;
    if ((n = read(client_fd, (void *) &msg, sizeof(msg_type_t))) == -1) {
        /* connection reset by client */
        if (errno == ECONNRESET) {
            printf("Client aborted connection\n");
            close(client_fd);
        } else
            perror("Read error\n");
        return -1;
    } else if (n == 0) {
        /* connection closed by client */
        printf("Client closed connection\n");
        return -1;
    } else {
        switch (msg) {
            case MSG_UP:
                peerList[++users_count] = malloc(sizeof(struct sockaddr_in));
                memcpy(peerList[users_count], client_addr, sizeof(*client_addr));
                printf("Added peer\t address: %s \t port: %d\n", inet_ntoa(peerList[users_count]->sin_addr),
                       peerList[users_count]->sin_port);
                msg_to_send.msg = CHK_MSG_UP;
                msg_to_send.peer_sock_addr = *client_addr;

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
                struct sockaddr_in *peerListToSend = malloc(users_count * sizeof(struct sockaddr_in));
                msgNumAll.msg = ACK_MSG_ALL;
                msgNumAll.noPeer = users_count;
                if (write(client_fd, &msgNumAll, sizeof(msgNumAll)) < 0) {
                    perror("Failed to send ACK_MSG_ALL message\n");
                    exit(-1);
                }

//            getAllPeers(peerListToSend);

                int n = users_count * sizeof(peerList[1]);
//                memcpy(&peerListToSend, peerList[1], n);

                struct sockaddr_in buff[users_count];
                memcpy(&buff, &peerList[1], n);
/*                for (int i = 1; i <= users_count; i++)
                    memcpy(&buff[i - 1], peerList[i], sizeof(peerList[i]));*/


                if (write(client_fd, buff, sizeof(buff)) < 0) {
                    perror("Failed to send ACK_MSG_ALL message\n");
                    exit(-1);
                }
            }
                break;

            default: {
                printf("Error! The client has sent an uknown type of packet\n");
            }
                break;

        }
        //close(client_fd);
    }
    return 1;

}

void addPeer(msg_peer_t *msg) {

}

/*void getAllPeers(struct sockaddr_in *msg_to_send) {
    int n = users_count * sizeof (peerList[1]);

    memcpy(&msg_to_send, &peerList[1], n);
    printf("\n");
}*/

/*
void add_peer_node(const struct sockaddr_in *new_peer_addr, struct sockaddr_in* posList) {
    struct sockaddr_in *new_node = malloc(sizeof(struct sockaddr_in));
    new_node->sin_family = AF_INET;

    memcpy(new_node, new_peer_addr, sizeof (struct sockaddr_in));
}
*/

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
/*    peer_addr.sin_addr = peerList[rnd]->sin_addr;
    peer_addr.sin_port = peerList[rnd]->sin_port;*/
    if (sendto(sockfd, pck, sizeof(pck), 0, (struct sockaddr *) &peer_addr, sizeof(peer_addr)) < 0) {
        perror("Sendto error when sending alarm message\n");
        exit(-1);
    }
}