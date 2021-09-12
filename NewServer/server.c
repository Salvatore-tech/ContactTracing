//
// Created by saso on 03/09/2021.
//

#include <unistd.h>
#include <resolv.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <arpa/inet.h>
#include "server.h"


int handlePacket(int client_fd) {
    msg_type_t msg;
    msg_peer_t msg_to_send;
    struct sockaddr_in peerListusers_count;
    socklen_t addr_len = sizeof (struct sockaddr_in);
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
        close(client_fd);
        users_count-=1;
        return -1;
    } else {
        switch (msg) {

            case MSG_UP: {
                peerList[users_count].sin_family = AF_INET;
                peerList[users_count].sin_addr.s_addr = htonl(INADDR_LOOPBACK);
                peerList[users_count].sin_port = htons(2000+users_count+1);

                msg_to_send.msg = CHK_MSG_UP;
                msg_to_send.peer_sock_addr = peerList[users_count];

                printf("Peer\t address: %s \t port: %d\n", inet_ntoa(peerList[users_count].sin_addr),
                       ntohs(peerList[users_count].sin_port));

                if (write(client_fd, &msg_to_send, sizeof(msg_to_send)) < 0) {
                    perror("Failed to send ACK message\n");
                    exit(-1);
                }

                users_count++;
                break;
            }


                // Delete user from the peerList in case of receiving MSG_DOWN
//            case MSG_DOWN: {
//                for (int i = 0; i < MAX_USERS; i++) {
//                    if (&peerList[i] && memcmp((void *) &peerList[i], (void *) client_addr, sizeof(client_addr)) == 0) {
//                        // free(peerList[i]);
//                        //peerList[i].peer_sock_addr;
//                        users_count--;
//                    }
//                }
//            }
//                break;
            case MSG_ALL: {
                msg_num_all_t msgNumAll;
                struct sockaddr_in buff[users_count];
                msgNumAll.msg = ACK_MSG_ALL;
                msgNumAll.noPeer = users_count;

                if (write(client_fd, &msgNumAll, sizeof(msgNumAll)) < 0) {
                    perror("Failed to send ACK_MSG_ALL message\n");
                    exit(-1);
                }

//                for (int i = 1; i <= users_count; i++)
//                    memcpy(&buff[i - 1], peerList[i], sizeof(peerList[i]));

                if (write(client_fd, peerList, users_count*sizeof (peerList[0])) < 0) {
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
