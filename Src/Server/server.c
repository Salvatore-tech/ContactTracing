//
// Created by saso on 03/09/2021.
//

#include <unistd.h>
#include <resolv.h>
#include <stdlib.h>
#include <errno.h>
#include <arpa/inet.h>
#include <pthread.h>
#include "server.h"

int launchDetachThreadToSignalPositivePeer() {
    pthread_t ntid;
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    return pthread_create(&ntid, &attr, signalPositive, NULL);
}

int handlePacket(int client_fd) {
    msg_type_t msg;
    ssize_t n;

    if ((n = read(client_fd, (void *) &msg, sizeof(msg_type_t))) == -1) {
        if (errno == ECONNRESET) {
            perror("Client aborted connection\n");
            close(client_fd);
            --users_count;
        } else
            perror("Read error\n");
        return -1;

    } else if (n == 0) {
        perror("Client closed connection\n");
        close(client_fd);
        --users_count;
        return -1;
    } else {
        switch (msg) {

            case MSG_UP: {
                msg_peer_t msg_to_send;

                peerList[users_count].sin_family = AF_INET;
                peerList[users_count].sin_addr.s_addr = htonl(INADDR_LOOPBACK);
                peerList[users_count].sin_port = htons(2000 + rand() % 8000);

                printf("Peer entered into the network: %s \t %d\n", inet_ntoa(peerList[users_count].sin_addr),
                       ntohs(peerList[users_count].sin_port));

                positivityAddr[users_count].sin_family = AF_INET;
                positivityAddr[users_count].sin_addr.s_addr = htonl(INADDR_LOOPBACK);
                positivityAddr[users_count].sin_port = htons(10000 + rand() % 2000);

                msg_to_send.msg = ACK_MSG_UP;
                msg_to_send.peerAddr = peerList[users_count];
                msg_to_send.positivityAddr = positivityAddr[users_count];

                if (fullWrite(client_fd, &msg_to_send, sizeof(msg_to_send)) != 0) {
                    perror("FullWrite error in sending ACK_MSG_UP\n");
                    exit(-1);
                }

                users_count++;
                break;
            }

            case MSG_ALL: {
                msg_num_all_t msgNumAll;
                msgNumAll.msg = ACK_MSG_ALL;
                msgNumAll.noPeer = users_count;

                if (fullWrite(client_fd, &msgNumAll, sizeof(msgNumAll)) != 0) {
                    perror("Failed to send ACK_MSG_ALL message\n");
                    exit(-1);
                }

                if (fullWrite(client_fd, peerList, users_count * sizeof(peerList[0])) != 0) {
                    perror("Failed to send ACK_MSG_ALL message\n");
                    exit(-1);
                }
            }
                break;

            default: {
                printf("Error! The client has sent an unknown type of packet\n");
            }
                break;
        }
    }
    return 1;
}

void *signalPositive() {
    int sock_fd;
    unsigned int i;
    msg_type_t msgToSend = MSG_POS;

    if ((sock_fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("socket");
        exit(-1);
    }

    while (1) {
        sleep(periodPositive);
        if (users_count >= 1) {
            i = rand() % users_count;// TODO edited

            if (sendto(sock_fd, &msgToSend, sizeof(msgToSend), 0, (struct sockaddr *) &positivityAddr[i],
                       sizeof(positivityAddr[i])) < 0) {
                perror("Sendto error when sending positivity message\n");
                exit(-1);
            }
        }
    }
}

ssize_t fullWrite(int fd, const void *buf, size_t count) {
    size_t nleft;
    ssize_t nwritten;
    nleft = count;
    while (nleft > 0) {             /* repeat until no left */
        if ((nwritten = write(fd, buf, nleft)) < 0) {
            if (errno == EINTR)
                continue;
            else
                exit(nwritten);
        }
        nleft -= nwritten;
        buf += nwritten;
    }
    return nleft;
}

int buildSocket(int enable_reuse, struct sockaddr_in *servSockaddr) {
    int list_sd;

    (*servSockaddr).sin_family = AF_INET;
    (*servSockaddr).sin_addr.s_addr = htonl(INADDR_ANY);
    (*servSockaddr).sin_port = htons(SRV_PORT);

    if ((list_sd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Could not create socket\n");
        exit(-1);
    }

    if (setsockopt(list_sd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, (void *) &enable_reuse, sizeof(enable_reuse)) < 0)
        perror("Setsockopt (SO_REUSEADDR) failed");

    if (bind(list_sd, (struct sockaddr *) servSockaddr, sizeof((*servSockaddr))) < 0) {
        perror("Bind error\n");
        exit(-1);
    }

    if (listen(list_sd, MAX_USERS) == -1) {
        perror("Listen error\n");
        exit(-1);
    }

    return list_sd;
}

void initPollStruct(int list_sd) {
    for (int i = 0; i < MAX_USERS; i++)
        client_poll_struct[i].fd = -1;

    client_poll_struct[0].fd = list_sd;
    client_poll_struct[0].events = POLLRDNORM;
}