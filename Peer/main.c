//
// Created by saso on 07/08/21.
//
#include "peer.h"
#include <stdio.h>
#include <pthread.h>
struct sockaddr_in neighbour_addr[NEIGH_NO];

int main(){
    int fd_server, fd_listener, fd_neighbour;
    pthread_t n_tid[5];
    char* neigh_id;
    size_t size_id = 65;
    fd_neighbour = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

    head = tail = NULL;

    fd_listener=0;
    enterIntoNetwork(&fd_server, &fd_listener);
    addNode();
    char* s = calloc(1, 65 + 1);
    generate_id(65, s);

    contactNeighbour();


    writeNeighbourID(s);




    //    neigh_id = contactNeighbour(fd_neighbour, &neighbour_addr, my_id);




    //Read bloccante sulla socket che usiamo per contattare il vicino


    return 0;
}

