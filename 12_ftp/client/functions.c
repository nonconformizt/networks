#include <unistd.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdio_ext.h>
#include <string.h>

#include "header.h"

int send_string(int socket, const char *sString);
void fatal_err(const char * msg);
void fill_addr(struct sockaddr_in * ctrl_addr, struct sockaddr_in * data_addr, const char * ip);

void clear_buf(char * str, int len)
{
    for (; --len >= 0;) str[len] = 0;
}


void fatal_err(const char * msg)
{
    fprintf(stderr, "[Error]: %s\n", msg);
    exit(1);
}


void fill_addr(struct sockaddr_in * ctrl_addr, struct sockaddr_in * data_addr, const char * ip)
{
    memset(ctrl_addr, 0, sizeof(*ctrl_addr));
    memset(ctrl_addr, 0, sizeof(*data_addr));

    ctrl_addr->sin_family = AF_INET;
    data_addr->sin_family = AF_INET;

    if(inet_pton(AF_INET, ip, &(ctrl_addr->sin_addr)) <= 0)  
        fatal_err("Invalid address!");
    data_addr->sin_addr.s_addr = 0;

    ctrl_addr->sin_port = htons((u_short) 21);
    data_addr->sin_port = htons((u_short) DATA_PORT);
}

int send_string(int socket, const char *str)
{
    return send(socket, str, strlen(str), 0);
}

void send_credentials(int socket, const char * user, const char * pass)
{
    char str[BUF_SIZE];
    char recv_buf[BUF_SIZE];
    int n = 0;

    // read greeting message from server
    // we don't really need it
    sleep(1); // f***ing hate this delay
    n = recv(socket, recv_buf, BUF_SIZE - 1, 0);


    sprintf(str, "USER %s\r\n", user);
    send_string(socket, str);

    n = recv(socket, recv_buf, BUF_SIZE - 1, 0);
    recv_buf[n] = 0;

    // printf("%s\n", recv_buf);
    if (strncmp("331", recv_buf, 3) == 0)
        printf("Username ok\n");
    else 
        fatal_err("Wrong username!");

    sprintf(str, "PASS %s\r\n", pass);
    send_string(socket, str);

    n = recv(socket, recv_buf, BUF_SIZE - 1, 0);
    recv_buf[n] = 0;

    // printf("%s\n", recv_buf);
    if (strncmp("230", recv_buf, 3) == 0)
        printf("Access granted!\n");
    else 
        fatal_err("Wrong password!");


}
