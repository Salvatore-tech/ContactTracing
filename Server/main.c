#include <stdio.h>
#include <pthread.h>
#include <sys/socket.h>
#include <arpa/inet.h> //inet_addr
#include <unistd.h>    //write
#include <string.h>    //strlen, puts
#include <stdio.h>
#include <stdlib.h> //exit
#include <netinet/in.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h> //inet_addr
#include "server.h"
#include <poll.h>
#include <errno.h>


int main() {
    int list_fd, client_fd, enable_reuse = 1, maxi = 0;
    int n_fd_ready;
    int conn_sd = 0;
    int i = 0;
    int j = 1;
    const int timeout = 500;
    struct sockaddr_in serv_sockaddr, client_sockaddr;
    socklen_t len = sizeof (client_sockaddr);
    struct pollfd client_poll_struct[MAX_USERS];

    struct sockaddr_in addr_buff[MAX_USERS] = {0};
//    struct sockaddr_in *peerList[MAX_USERS] = {0};

    if ((list_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Could not create socket\n");
        exit(-1);
    };
    memset((char *)&serv_sockaddr, 0, sizeof(serv_sockaddr));
    serv_sockaddr.sin_family = AF_INET;
    serv_sockaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_sockaddr.sin_port = htons(SRV_PORT);

    if (setsockopt(list_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &enable_reuse, sizeof(int)) < 0)
        perror("setsockopt(SO_REUSEADDR) failed");

    if (bind(list_fd, (struct sockaddr *) &serv_sockaddr, sizeof(serv_sockaddr)) < 0) {
        perror("Bind error\n");
        exit(-1);
    }

    if (listen(list_fd, MAX_USERS) == -1) {
        perror("Listen error\n");
        exit(-1);
    }


    for (int i = 0; i < MAX_USERS; i++)
        client_poll_struct[i].fd = -1;
    client_poll_struct[0].fd = list_fd;
    client_poll_struct[0].events = POLLRDNORM;
    while (1) {
        n_fd_ready = poll(client_poll_struct, maxi + 1, timeout);
        if (client_poll_struct[0].revents & POLLRDNORM) {
            if ((conn_sd = accept(list_fd, (struct sockaddr *) &client_sockaddr, &len)) < 0) {
                perror("Accept error\n");
                exit(-1);
            }
            for (i = 1; i < MAX_USERS; i++)
                if (client_poll_struct[i].fd < 0) {
                    client_poll_struct[i].fd = conn_sd;
                    client_poll_struct[i].events = POLLRDNORM;

                    addr_buff[i] = client_sockaddr;
                    break;
                }
            if (i == MAX_USERS)
                perror("Too many clients");
            if (i > maxi)
                maxi = i;
            if (--n_fd_ready <= 0)
                continue;
        }
        for (int j = 1; j <= maxi; j++) {
            if (client_poll_struct[j].fd && client_poll_struct[j].revents & (POLLERR | POLLRDNORM)) {
                if (handlePacket(client_poll_struct[j].fd, peerList, &addr_buff[j]) < 0)
                    client_poll_struct[j].fd = -1;
//                close(client_poll_struct[j].fd);
                if (check_signal(5))
                    send_signal();
            }
        }
    }
    for (int i = 0; i < users_count; i++)
        free(&peerList[i]);
    return 0;
}
