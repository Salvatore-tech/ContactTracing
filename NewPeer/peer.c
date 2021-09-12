//
// Created by saso on 03/09/2021.
//
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <stdlib.h>
#include "header.h"
#include "peer.h"
#include "ds_peer.h"

struct sockaddr_in enterIntoNetwork(int *fd_server) {
    struct sockaddr_in serv_addr;
    msg_type_t send_message = MSG_UP;
    msg_peer_t recv_message;

    if ((*fd_server = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Could not create socket, there is something wrong with your system");
        exit(-1);
    }

//    memset((char *) &serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(SRV_PORT); //port num defined in header
    socklen_t len = sizeof(serv_addr);

    if (connect(*fd_server, (struct sockaddr *) &serv_addr, len) < 0) {
        perror("Connect to server failed\n");
        exit(-1);
    }

    if (write(*fd_server, &send_message, sizeof(send_message)) < 0) {
        perror("Write error\n");
        exit(-1);
    }

//    memset((msg_peer_t*)&recv_message, 0, sizeof (recv_message));
    if (read(*fd_server, &recv_message, sizeof(recv_message)) < 0) {
        perror("Read error\n");
        exit(-1);
    }

    if (recv_message.msg == CHK_MSG_UP) {

        pthread_attr_t attr;
        pthread_t ntid;
        struct sockaddr_in addr;

//        printf("Peer address: %s \t port: %d\n", inet_ntoa(recv_message.peer_sock_addr.sin_addr),
//               ntohs(recv_message.peer_sock_addr.sin_port));
        return recv_message.peer_sock_addr;
    }
}

void contactNeighbour(int fd_server) {
    msg_type_t msg_request_list = MSG_ALL;
    msg_num_all_t msgNumAll;

    if (write(fd_server, &msg_request_list, sizeof(msg_request_list)) < 0) {
        printf("%s", strerror(errno));
        perror("Write error\n");
        exit(-1);
    }
    if (read(fd_server, &msgNumAll, sizeof(msgNumAll)) < 0) {
        perror("Read error\n");
        exit(-1);
    }

    if (msgNumAll.msg == ACK_MSG_ALL) {

        int n;
        if ((n = msgNumAll.noPeer * sizeof(struct sockaddr_in)) > 0) {
            struct sockaddr_in msg_peer_to_recv[msgNumAll.noPeer];
            if (read(fd_server, msg_peer_to_recv, n) < 0) {
                perror("Read error\n");
                exit(-1);
            }

            for (int i = 0; i < msgNumAll.noPeer; i++)
                printf("Peer %d\t address: %s \t port: %d\n", i, inet_ntoa(msg_peer_to_recv[i].sin_addr),
                       ntohs(msg_peer_to_recv[i].sin_port));

            printf("\nSelect the peer to connect: ");
            int input;
            scanf("%d", &input);
            while (input > msgNumAll.noPeer - 1 || input < 0) {
                printf("\nInvalid choice, enter a valid number: ");
                scanf("%d", &input);
            }

            sendId(&msg_peer_to_recv[input]);


        } else
            perror("None into the network, can't communicate!\n");
    }
}

void *sendId(void *args) {
    struct sockaddr_in addr = *(struct sockaddr_in *) args;
    msg_neigh_id_t msgNeigh, msgAckNeigh;


    int fd_neighbour;
    if ((fd_neighbour = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket");
        exit(-1);
    }

    msgNeigh.msg = MSG_CONN;
    memcpy(msgNeigh.id, tail->id, ID_LEN);
    int t = strlen(msgNeigh.id);


//    printf("Peer\t address: %s \t port: %d\n", inet_ntoa(addr.sin_addr),
//           addr.sin_port);

    socklen_t len = sizeof(addr);
    size_t sizeNeigh = sizeof(msgNeigh);

    connect(fd_neighbour, (struct sockaddr *) &addr, sizeof(addr));

    if (write(fd_neighbour, &msgNeigh, sizeof(msgNeigh)) < 0) {
        printf("Write error %s\n", strerror(errno));
        exit(-1);
    }

    if (read(fd_neighbour, &msgAckNeigh, sizeof(msgAckNeigh)) < 0) {
        perror("Read error\n");
        exit(-1);
    }

    if (msgAckNeigh.msg == ACK_CONN) {
        writeNeighbourID(msgAckNeigh.id);
    }
}


void *recvId(void *args) {
    int fd_listener, conn_sd;
    msg_neigh_id_t msg_to_send, msg_to_rcv;
    struct sockaddr_in addr = *(struct sockaddr_in *) args;
    struct sockaddr_in senderAddr;
    socklen_t addrLen = sizeof(senderAddr);

    if ((fd_listener = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket error\n");
        exit(-1);
    }

//    printf("\nRecvId -> Peer address: %hu %s \t port: %d\n",addr.sin_family, inet_ntoa(addr.sin_addr),
//           ntohs(addr.sin_port));

    int enable_reuse = 1;
    if (setsockopt(fd_listener, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &enable_reuse, sizeof(enable_reuse)) < 0)
        perror("setsockopt(SO_REUSEADDR) failed");

    if (bind(fd_listener, (struct sockaddr *) &addr, sizeof(addr)) < 0) {
        perror("Bind error: recvId\n");
        exit(-1);
    }

    msg_to_send.msg = ACK_CONN;
    memcpy(msg_to_send.id, tail->id, ID_LEN);

    if (listen(fd_listener, 10) < 0) {
        perror("Listen error\n");
        exit(-1);
    }

    while (1) {

        if ((conn_sd = accept(fd_listener, (struct sockaddr *) &senderAddr, &addrLen)) < 0) {
            perror("Accept error\n");
            exit(-1);
        }

        read(conn_sd, &msg_to_rcv, sizeof(msg_to_rcv));

        printf("Received an incoming message\n");


        if (msg_to_rcv.msg == MSG_CONN) {
            writeNeighbourID(msg_to_rcv.id);

            if (write(conn_sd, &msg_to_send, sizeof(msg_to_send)) < 0) {
                perror("Write error\n");
            }
        }
        close(conn_sd);
    }
}

void* listBroadcast() {
    int sock, n;
    int enable = 1;
    struct sockaddr_in addr, in;
     msg_neigh_id_t pck_received;
    socklen_t len = sizeof(in);

    if ((sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) {
        perror("socket");
        exit(1);
    }
    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(enable));

    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(BROAD_PORT);

    if (bind(sock, (struct sockaddr *) &addr, sizeof(addr)) < 0) {
        perror("bind");
        exit(1);
    }

    size_t l = sizeof(pck_received);
    if (recvfrom(sock, &pck_received, l, 0, NULL, NULL) < 0) {
        perror("Error receiving the broadcast pck\n");
        exit(-1);
    }

    if (pck_received.msg == MSG_BRD) {
        char *token;
        token = strtok(pck_received.id, "_");
        while (token) {
            if (searchNeighbour(token) == 1) {
                printf("I'm the peer %s and i have been in touch with someone positive\n", token);
                break;
            }
            token = strtok(NULL, "_");
        }
    }
}

void sendBroadcast() {
    int sock_fd;
    int enable = 1;
    msg_neigh_id_t msg_to_send;
    struct sockaddr_in addr;

    if ((sock_fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) {
        fprintf(stderr, "socket error\n");
        exit(1);
    }

    setsockopt(sock_fd, SOL_SOCKET, SO_BROADCAST, &enable, sizeof(enable));

    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_BROADCAST;
    addr.sin_port = htons(BROAD_PORT);

    msg_to_send.msg = MSG_BRD;
    memcpy(msg_to_send.id, tail->neigh_ids, ID_LEN);


    //TODO: send the entire list and not only the first id

    size_t addrLen = sizeof(addr);
    if (sendto(sock_fd, &msg_to_send, sizeof(msg_to_send), 0, (struct sockaddr *) &addr, addrLen) < 0) { /* error condition, stop client */
        printf("Errore in lettura: %s\n", strerror(errno));
        exit(-1);
    }
}

void generate_id(size_t size, char *str) {
    if (str) {
        size_t n;
        static char charset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
        if (size) {
            --size;
            for (n = 0; n < size-1; n++) {
                int key = rand() % (int) (sizeof charset - 1);
                str[n] = charset[key];
            }
            str[n++] = '_';
            str[n] = '\0';
        }
    }
}

