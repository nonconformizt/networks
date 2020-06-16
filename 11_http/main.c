#include <unistd.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <stdio.h>

#include "./lib/req_parser.c"

#define PORT 8080


int main(int argc, char *argv[])
{
    int server_fd, newsock_fd;
    int termianate = 0;
    struct sockaddr_in serv_addr = {}, client_addr = {};
    int port, pid, client_adrlen;

    port = (argc == 1) ? PORT : atoi(argv[1]);

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        fprintf(stderr, "Can't create socket!\n");
        exit(1);
    }

    // Заполняем структуру адресов
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = 0;
    serv_addr.sin_port = htons((u_short) port);
    // Связываем сокет с заданным сетевым интерфесом и портом
    if (bind(server_fd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) 
    {
        fprintf(stderr, "Can't bind on port %d\n", port);
        exit(1);
    }


    if (listen(server_fd, 100) < 0) {
        fprintf(stderr, "Cannot listen on port %d\n", port);
        exit(1);
    }

    fprintf(stdout, "Server is listening on port %d\n", port);

    while (!termianate)
    {
        newsock_fd = accept(server_fd, (struct sockaddr *) &client_addr, &client_adrlen);
        if (newsock_fd < 0) {
            perror("Can't accept client connetion:");
            exit(1);
        }

        pid = fork();

        if (!pid) {
            close(server_fd);
            handle_connection(newsock_fd);
            exit(EXIT_SUCCESS);
        } else {
            close(newsock_fd);
        }

    }

}