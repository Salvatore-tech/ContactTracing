#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include "common.h"
#include "peer.h"
#include "ds_peer.h"

node_t *head = NULL;
node_t *tail = NULL;

int main() {
    int fd_server;
    pthread_t n_tid[4];
    int choice;
    struct sockaddr_in positivityAddr;
    srand((unsigned) time(NULL));

    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

    struct sockaddr_in myAddr = enterIntoNetwork(&fd_server, &positivityAddr);

    pthread_create(&n_tid[0], &attr, addNodeToList, NULL); // Generate a new id every 60 sec
    pthread_create(&n_tid[1], &attr, recvId, (void *) &myAddr); // Listen for an incoming message from another peer
    pthread_create(&n_tid[2], &attr, listBroadcast, NULL); // Listen for an incoming broadcast message
    pthread_create(&n_tid[3], &attr, sendBroadcast, &positivityAddr); // Send a broadcast message, if it is infected

    do {
        choice = generateMenu();

        switch (choice) {
            case 1:
                contactNeighbour(fd_server, &myAddr);
                break;

            case 2:
                printPeersContacted();
                break;

            case 3:
                printGeneratedId();
                break;

            case 0:
                printf("Exiting from CT network\n");
                break;

            default:
                printf("Invalid number\nn");
        }

    } while (choice != 0);

    close(fd_server);
    deleteNodeList();
    return 0;
}
