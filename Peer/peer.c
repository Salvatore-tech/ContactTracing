//
// Created by saso on 07/08/21.
//

#include "peer.h"
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include "ds_peer.h"
#include "header.h"

int enterIntoNetwork(int *fd_server, int *fd_listener) {
    struct sockaddr_in serv_addr;
    msg_peer_t send_message, recv_message;
    send_message.msg = MSG_UP;
    *fd_server = socket(AF_INET, SOCK_STREAM, 0);
    if (*fd_server == -1) {
        perror("Could not create socket, there is something wrong with your system");
        exit(-1);
    }
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(SRV_PORT); //port num defined in chat.h
    socklen_t len = sizeof(serv_addr);
    if (connect(*fd_server, (struct sockaddr *) &serv_addr, len) < 0) {
        perror("Connect to server failed\n");
        exit(-1);
    }

    if (write(*fd_server, &send_message, sizeof(send_message)) < 0) {
        perror("Write error\n");
        exit(-1);
    }
    if (read(*fd_server, &recv_message, sizeof(msg_type_t)) < 0) {
        perror("Read error\n");
        exit(-1);
    }
    if (send_message.msg == CHK_MSG_UP) {
        if (socket(AF_INET, SOCK_DGRAM, 0) < 0) {
            perror("Socket error\n");
            exit(-1);
        }
        if (bind(*fd_listener, (struct sockaddr *) &recv_message.peer_sock_addr, sizeof(recv_message.peer_sock_addr)) <
            0) {
            perror("Bind error\n");
            exit(-1);
        }
    }
    return 1;
}


// Il peer richiede al server di inviargli un vicino
void requestNeighbour(int fd_server, struct sockaddr_in *neigh_addr) {
    msg_peer_t *recv_message;
    msg_neigh_t send_message;
    send_message.msg = MSG_GET_NEIGH;
    if (write(fd_server, &send_message, sizeof(send_message)) < 0) {
        perror("Write error\n");
        exit(-1);
    }
    if (read(fd_server, &recv_message, sizeof(msg_type_t)) < 0) {
        perror("Read error\n");
        exit(-1);
    }
    *neigh_addr = recv_message->peer_sock_addr;
}

char *contactNeighbour(int fd_neighbour, const struct sockaddr_in *neigh_addr, char *my_id, node_t **tail_id) {
    char *neigh_id = malloc(sizeof(my_id));
    msg_neigh_id_t msg_to_send, msg_to_rcv;
    msg_to_send.id = my_id;
    msg_to_send.msg = MSG_CONN;
    if (sendto(fd_neighbour, my_id, sizeof(my_id), 0, (const struct sockaddr *) neigh_addr, sizeof(*neigh_addr))) {
        perror("Sendto error\n");
        exit(-1);
    }
    if (recvfrom(fd_neighbour, &msg_to_rcv, sizeof(msg_to_rcv), 0, NULL, NULL) < 0) {
        perror("Recvfrom error\n");
        exit(-1);
    }
    // TODO: check neigh_id validity on return
    if (msg_to_rcv.msg == ACK_CONN) {
        // writeNeighbourID((*tail_id)->neigh_ids, msg_to_rcv.id, 0);
    }
}

void readIncomingMessage(int fd_listener, node_t **tail_id) {
    // TODO: read from fd_listener and add the string ids to the current node. Da definire il comportamento
    msg_neigh_id_t msg_to_send, msg_to_rcv;
    struct sockaddr *addr;
    socklen_t len = sizeof(addr);
    if (recvfrom(fd_listener, &msg_to_rcv, sizeof(msg_to_rcv), 0, addr, &len) < 0) {
        perror("Error receiving the message\n");
        exit(-1);
    }
    if (msg_to_rcv.msg == MSG_CONN) {
        //writeNeighbourID((*tail_id)->neigh_ids, msg_to_rcv.id, 0);
        msg_to_send.msg = ACK_CONN;
        char *my_id = "";
        int sock_fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
        msg_to_send.id = my_id;
        if (sendto(sock_fd, my_id, sizeof(my_id), 0, (const struct sockaddr *) addr, &len < 0)) {
            perror("Sendto error\n");
        }
    }

}

void listBroadcast(int *brd_fd, const node_t *head_id, const node_t *tail_id) {
    int sock, n;
    int enable = 1;
    struct sockaddr_in addr, in;
    msg_brd_t *pck_received;
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

    if (recvfrom(sock, &pck_received, sizeof(pck_received), 0, NULL, NULL) < 0) {
        perror("Error receiving the broadcast pck\n");
        exit(-1);
    }

    if (pck_received->msg == MSG_BRD) {
        char *token;
        token = strtok(pck_received->ids, ":");
        while (token) {
            if (searchNeighbour(token) == 1) {
                printf("I'm the peer %s and i have been in touch with someone positive\n", token);
                break;
            }
            token = strtok(NULL, ":");
        }
    }
}

void sendBroadcast(char **id) {
    int sock_fd, n_sent;
    int enable = 1;
    msg_brd_t msg_to_send;
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
    memcpy(&msg_to_send.msg, id, sizeof(*id));
    //TODO: wait for the server's signal
    n_sent = sendto(sock_fd, msg_to_send.ids, sizeof(msg_to_send), 0, (struct sockaddr *) &addr, sizeof(addr));
    if (n_sent < 0) { /* error condition, stop client */
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
            for (n = 0; n < size - 1; n++) {
                int key = rand() % (int) (sizeof charset - 1);
                str[n] = charset[key];
            }
            str[n++] = '_';
            str[n] = '\0';
        }
    }
}