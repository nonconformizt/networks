#include <unistd.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <stdio.h>

#include "./functions.c"


int main(int argc, char *argv[])
{
    if (argc < 4)
        fatal_err("Too few arguments!");
    
    const char * user = argv[1];
    const char * pass = argv[2];
    const char * host = argv[3];
    
    int ctrl_fd, data_fd;
    ctrl_fd = socket(AF_INET, SOCK_STREAM, 0);
    data_fd = socket(AF_INET, SOCK_STREAM, 0);

    if (!ctrl_fd || !data_fd)
        fatal_err("Can't create socket");

    struct sockaddr_in ctrl_addr, data_addr;
    fill_addr(&ctrl_addr, &data_addr, host);


    if (connect(ctrl_fd, (struct sockaddr *) &ctrl_addr, sizeof(ctrl_addr)) != 0) 
        fatal_err("Connection Failed!"); 

    if (listen(data_fd, 1) != 0)
        fatal_err("Cannot listen on port!");
    
    printf("Connected to server!\n");


    send_credentials(ctrl_fd, user, pass);


}