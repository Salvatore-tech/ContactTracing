#include <stdio.h>
#include "peer.h"
#include "ds_peer.h"
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>

node_t *head = NULL;
node_t *tail = NULL;

int main() {
    int fd_server;
    pthread_t n_tid[5];
    int choice = 0;
    struct sockaddr_in check_pos_addr;

    srand(time(NULL));

    struct sockaddr_in myAddr = enterIntoNetwork(&fd_server, &check_pos_addr);

    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

    pthread_create(&n_tid[0], &attr, addNode, NULL);
    pthread_create(&n_tid[1], &attr, recvId, (void *) &myAddr); // Listen for an incoming message from another peer
    pthread_create(&n_tid[2], &attr, listBroadcast, NULL);
    pthread_create(&n_tid[3], &attr, sendBroadcast, &check_pos_addr);

    do {
        printf("\n1) Contact a peer");
        printf("\n2) View contacts");
        printf("\n3) Show generated id");
        printf("\n0) Terminate\n");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                contactNeighbour(fd_server, &myAddr);
                break;

            case 2:
                printList(head);
                break;

            case 3:
                printMyId();
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
