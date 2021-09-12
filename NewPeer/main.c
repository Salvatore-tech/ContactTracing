#include <stdio.h>

#include "peer.h"
#include "ds_peer.h"
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>


struct sockaddr_in neighbour_addr[NEIGH_NO];

int main() {
    int fd_server;
    pthread_t n_tid[5];
    int scelta = 0;
    pthread_attr_t attr;
    head = tail = NULL;

    srand(time(0));
    addNode();

    struct sockaddr_in s = enterIntoNetwork(&fd_server);

    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    pthread_create(&n_tid[0], &attr, recvId, (void *) &s); // Listen for an incoming message from another peer
    pthread_create(&n_tid[1], &attr, listBroadcast, NULL);


    do {
        printf("\n1) Contatta un peer");
        printf("\n2) Visualizza contatti avvenuti");
        printf("\n3) Stampa l'id generato");
        printf("\n4) Invio broadcast");
        printf("\n0) Termina\n");
        scanf("%d", &scelta);

        switch (scelta) {
            case 1:
                contactNeighbour(fd_server);
                break;

            case 2:
                printList(head);
                break;

            case 3:
                printMyId(head);
                break;

            case 4:
                sendBroadcast();
                break;

            default:
                printf("Numero non valido n");
        }

    } while (scelta != 0);

    close(fd_server);
    return 0;
}
