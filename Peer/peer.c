//
// Created by saso on 03/09/2021.
//
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <limits.h>
#include "common.h"
#include "peer.h"
#include "ds_peer.h"

struct sockaddr_in enterIntoNetwork(int *fd_server, struct sockaddr_in *checkPosAddr) {
    struct sockaddr_in serv_addr;
    msg_type_t send_message = MSG_UP;
    msg_peer_t recv_message;

    if ((*fd_server = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Could not create socket\n");
        exit(-1);
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(SRV_PORT);
    socklen_t len = sizeof(serv_addr);

    if (connect(*fd_server, (struct sockaddr *) &serv_addr, len) < 0) {
        perror("Connect to server failed\n");
        exit(-1);
    }

    if (fullWrite(*fd_server, &send_message, sizeof(send_message)) < 0) {
        perror("Write error\n");
        exit(-1);
    }

    if (fullRead(*fd_server, &recv_message, sizeof(recv_message)) < 0) {
        perror("Read error\n");
        exit(-1);
    }

    if (recv_message.msg == ACK_MSG_UP) {
        *checkPosAddr = recv_message.check_pos_addr;
    }
    return recv_message.peer_sock_addr;
}

void contactNeighbour(int fd_server, const struct sockaddr_in *myAddr) {
    int i;
    int idxToSkip = INT_MAX;
    msg_num_all_t msgNumAll;
    msg_type_t msg_request_list = MSG_ALL;

    if (fullWrite(fd_server, &msg_request_list, sizeof(msg_request_list)) < 0) {
        perror("Fullwrite error in contactNeighbour\n");
        exit(-1);
    }

    if (fullRead(fd_server, &msgNumAll, sizeof(msgNumAll)) < 0) {
        perror("Fullread error in contactNeighbour\n");
        exit(-1);
    }

    if (msgNumAll.noPeer == 1) {
        printf("There is not another peer available to communicate\n");
        return;
    }

    if (msgNumAll.msg == ACK_MSG_ALL) {
        int n;
        if ((n = msgNumAll.noPeer * sizeof(struct sockaddr_in)) > 0) {
            struct sockaddr_in msg_peer_to_recv[msgNumAll.noPeer];
            if (fullRead(fd_server, msg_peer_to_recv, n) < 0) {
                perror("Read error\n");
                exit(-1);
            }

            printf("\n");
            for (i = 0; i < msgNumAll.noPeer; i++) {
                if (memcmp(myAddr, &msg_peer_to_recv[i], sizeof(*myAddr)) == 0) {
                    idxToSkip = i;
                    printf("Peer %d\t address: %s \t port: %d \t (ourself, do not insert this index)\n", i,
                           inet_ntoa(msg_peer_to_recv[i].sin_addr),
                           ntohs(msg_peer_to_recv[i].sin_port));

                } else
                    printf("Peer %d\t address: %s \t port: %d\n", i, inet_ntoa(msg_peer_to_recv[i].sin_addr),
                           ntohs(msg_peer_to_recv[i].sin_port));
            }

            printf("\nSelect the peer to connect: ");
            int input;
            scanf("%d", &input);
            while (input > msgNumAll.noPeer || input < 0 || input == idxToSkip) {
                printf("\nInvalid choice, enter a valid number: ");
                scanf("%d", &input);
            }

            sendId(&msg_peer_to_recv[input]);
            printf("\nContact has happened\n");
        }
    }
}

void sendId(const struct sockaddr_in *peerToContact) {
    int fd_neighbour;
    msg_neigh_id_t msgNeigh, msgAckNeigh;

    if ((fd_neighbour = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("socket");
        exit(-1);
    }

    msgNeigh.msg = MSG_CONN;
    memcpy(msgNeigh.id, tail->id, ID_LEN);

    if (sendto(fd_neighbour, &msgNeigh, sizeof(msgNeigh), 0, (struct sockaddr *) peerToContact,
               sizeof(*peerToContact)) < 0) {
        printf("Sendto error %s\n", strerror(errno));
        exit(-1);
    }

    if (recvfrom(fd_neighbour, &msgAckNeigh, sizeof(msgAckNeigh), 0, NULL, NULL) < 0) {
        perror("Recvfrom error\n");
        exit(-1);
    }

    if (msgAckNeigh.msg == ACK_CONN) {
        writeNeighbourID(msgAckNeigh.id);
    }
}

_Noreturn void *recvId(void *args) {
    int fd_listener;
    int enable_reuse = 1;
    msg_neigh_id_t msg_to_send, msg_to_rcv;
    struct sockaddr_in addr = *(struct sockaddr_in *) args;
    struct sockaddr_in senderAddr;
    socklen_t addrLen = sizeof(senderAddr);

    if ((fd_listener = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Socket error\n");
        exit(-1);
    }

    if (setsockopt(fd_listener, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &enable_reuse, sizeof(enable_reuse)) < 0)
        perror("setsockopt(SO_REUSEADDR) failed");

    if (bind(fd_listener, (struct sockaddr *) &addr, sizeof(addr)) < 0) {
        perror("Bind error: recvId\n");
        exit(-1);
    }

    while (1) {
        if (recvfrom(fd_listener, &msg_to_rcv, sizeof(msg_to_rcv), 0, (struct sockaddr *) &senderAddr, &addrLen) < 0) {
            perror("Read error\n");
            exit(-1);
        }

        if (msg_to_rcv.msg == MSG_CONN) {
            printf("\nContact has happened\n");
            writeNeighbourID(msg_to_rcv.id);

            msg_to_send.msg = ACK_CONN;
            memcpy(msg_to_send.id, tail->id, sizeof(tail->id));

            if (sendto(fd_listener, &msg_to_send, sizeof(msg_to_send), 0, (struct sockaddr *) &senderAddr, addrLen) <
                0) {
                printf("Sendto error: %s\n", strerror(errno));
                exit(-1);
            }
        }
    }
}

_Noreturn void *sendBroadcast(void *args) {
    int sock_fd, list_fd;
    struct sockaddr_in check_pos_addr = *(struct sockaddr_in *) args;
    msg_neigh_id_t msg_to_send;
    msg_type_t msgToRecv;
    struct sockaddr_in myAddr, broadAddr;

    myAddr.sin_family = AF_INET;
    myAddr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    myAddr.sin_port = 0;

    broadAddr.sin_family = AF_INET;
    broadAddr.sin_addr.s_addr = htonl(INADDR_BROADCAST);
    broadAddr.sin_port = htons(BROAD_PORT);

    sock_fd = buildSocket(1, &myAddr);

    list_fd = buildSocket(0, &check_pos_addr);

    while (1) {
        if (recvfrom(list_fd, &msgToRecv, sizeof(msgToRecv), 0, NULL, NULL) < 0) {
            perror("Recvfrom positivity error\n");
            exit(-1);
        }

        if (msgToRecv == MSG_POS) {
            printf("\n *** Received a positive result ***\n");

            if (tail->neigh_ids[0]) { // If I have not been in touch with anyone else, it does not need to send a broadcast pck
                msg_to_send.msg = MSG_BRD;
                memcpy(msg_to_send.id, tail->neigh_ids, ID_LEN - 1);

                char *id_copy = strdup(tail->neigh_ids);

                for (char *p = strtok(id_copy, delimiter); p != NULL; p = strtok(NULL, delimiter)) {
                    memcpy(msg_to_send.id, p, ID_LEN - 1);
                    if (sendto(sock_fd, &msg_to_send, sizeof(msg_to_send), 0, (struct sockaddr *) &broadAddr,
                               sizeof(broadAddr)) < 0) {
                        printf("Errore in lettura: %s\n", strerror(errno));
                        exit(-1);
                    }
                }
                free(id_copy);
            }
        }
    }
}

_Noreturn void *listBroadcast() {
    int sock_fd;
    int so_reuseaddr = 1;
    int toSkip = 0;
    struct sockaddr_in myAddr;
    msg_neigh_id_t pck_received;
    struct sockaddr_in positiveSender, previousSender;
    socklen_t len = sizeof(struct sockaddr_in);

    if ((sock_fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Socket error\n");
        exit(-1);
    }

    myAddr.sin_family = AF_INET;
    myAddr.sin_addr.s_addr = htonl(INADDR_BROADCAST);
    myAddr.sin_port = (in_port_t) htons(BROAD_PORT);

    if (setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR, &so_reuseaddr, sizeof so_reuseaddr) < 0)
        perror("setsockopt(SO_REUSEADDR)");

    if (bind(sock_fd, (struct sockaddr *) &myAddr, sizeof(myAddr)) < 0)
        perror("Bind error\n");

    while (1) {
        if (recvfrom(sock_fd, &pck_received, sizeof(pck_received), 0, (struct sockaddr *) &positiveSender, &len) < 0) {
            perror("Error receiving the broadcast pck\n");
            exit(-1);
        }

        if (pck_received.msg == MSG_BRD) {
            char *token;
            token = strtok(pck_received.id, delimiter);

            if (token) {
                if (searchNeighbour(token, &previousSender, &positiveSender, &toSkip) == 1) {
                    printf("I'm the peer %s and i have been in touch with someone positive\n", token);
                    break;
                }
                if (toSkip == 1)
                    break;
            }

            toSkip = 0;
        }
    }
}

void generate_id(size_t size, char *str) {
    if (str && size > 0) {
        size_t n;
        static char charset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
        for (n = 0; n < size - 2; n++) {
            int key = rand() % (int) (sizeof charset - 1);
            str[n] = charset[key];
        }
        str[n++] = delimiter;
        str[n] = '\0';
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

ssize_t fullRead(int fd, void *buf, size_t count) {
    size_t nleft;
    ssize_t nread;
    nleft = count;
    while (nleft > 0) {             /* repeat until no left */
        if ((nread = read(fd, buf, nleft)) < 0) {
            if (errno == EINTR)
                continue;
            else
                exit(nread);
        } else if (nread == 0)
            break;
        nleft -= nread;
        buf += nread;
    }
    return (nleft);
}

int buildSocket(int so_broadcast, const struct sockaddr_in *addr) {
    int sock_fd;

    if ((sock_fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Socket error sendBroadcast\n");
        exit(-1);
    }

    if (so_broadcast == 1)
        if (setsockopt(sock_fd, SOL_SOCKET, SO_BROADCAST, &so_broadcast, sizeof(so_broadcast)) < 0)
            perror("setsockopt(SO_BROADCAST)");

    if (bind(sock_fd, (struct sockaddr *) addr, sizeof(*addr)) < 0)
        perror("bind()");

    return sock_fd;
}

