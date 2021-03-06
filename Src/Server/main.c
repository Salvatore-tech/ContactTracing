#include <stdio.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <poll.h>
#include "server.h"

struct pollfd client_poll_struct[MAX_USERS] = {0};

int main() {
    int list_sd, conn_sd, n_fd_ready, i;
    int max_sd = 0;
    int enable_reuse = 1;
    struct sockaddr_in servSockaddr;

    list_sd = buildSocket(enable_reuse, &servSockaddr);

    initPollStruct(list_sd);

    if (launchDetachThreadToSignalPositivePeer() != 0) {
        perror("Error starting thread detached\n");
        exit(-1);
    }

    printf("Server's running... waiting for incoming requests\n\n");

    while (1) {
        n_fd_ready = poll(client_poll_struct, max_sd + 1, TIMEOUT);
        if (client_poll_struct[0].revents & POLLRDNORM) {

            if ((conn_sd = accept(list_sd, (struct sockaddr *) NULL, NULL)) < 0) {
                perror("Accept error\n");
                exit(-1);
            }

            for (i = 1; i < MAX_USERS; i++) // Add the new fd to the first empty position of the array
                if (client_poll_struct[i].fd < 0) {
                    client_poll_struct[i].fd = conn_sd;
                    client_poll_struct[i].events = POLLRDNORM;
                    break;
                }

            if (i == MAX_USERS)
                perror("Too many clients");

            if (i > max_sd) // Update the greatest fd
                max_sd = i;

            if (--n_fd_ready <= 0) // Continue until there are pending connections
                continue;
        }
        // Loop through the fd previously inserted and handle the requests based on their type
        for (int j = 1; j <= max_sd; j++) {
            if (client_poll_struct[j].fd && client_poll_struct[j].revents & (POLLERR | POLLRDNORM)) {
                if (handlePacket(client_poll_struct[j].fd) < 0)
                    client_poll_struct[j].fd = -1;
            }
        }
    }
}

