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

    ctrl_addr->sin_port = htons((u_short) 21);
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

    if (strncmp("331", recv_buf, 3) == 0)
        printf("Username ok\n");
    else 
        fatal_err("Wrong username!");

    sprintf(str, "PASS %s\r\n", pass);
    send_string(socket, str);

    sleep(1); // f***ing hate this delay

    n = recv(socket, recv_buf, BUF_SIZE - 1, 0);
    recv_buf[n] = 0;

    if (strncmp("230", recv_buf, 3) == 0)
        printf("Access granted!\n");
    else 
        fatal_err("Wrong password!");


}

void setup_data_conn(int *data_fd, struct sockaddr_in * data_addr, int ctrl_fd)
{
    char str[BUF_SIZE];
    char recv_buf[BUF_SIZE];
    char * ptr;
    int n = 0, count = 0;
    u_short port;
    char serv_addr_port[BUF_SIZE] = {};
    char serv_addr_str[BUF_SIZE] = {};

    close(*data_fd);
    *data_fd = socket(AF_INET, SOCK_STREAM, 0);

    sprintf(str, "PASV\r\n");
    send_string(ctrl_fd, str);

    n = recv(ctrl_fd, recv_buf, BUF_SIZE - 1, 0);
    recv_buf[n] = 0;

    // GET ip and port from server response
    sscanf(recv_buf, "227 Entering Passive Mode (%[0-9,]s).\r\n", serv_addr_port);
    printf("Fetched string: %s\n", serv_addr_port);

    ptr = strtok(serv_addr_port, ",");
    if (ptr == NULL)
        fatal_err("Cannot parse connection string!");

    while (ptr != NULL)
    {
        if (count < 3) {
            strcat(serv_addr_str, ptr);
            strcat(serv_addr_str, ".");
        }
        else if (count == 3)
            strcat(serv_addr_str, ptr);
        else if (count == 4)
            port = atoi(ptr) * 256;
        else if (count == 5)
            port += atoi(ptr);

        ptr = strtok(NULL, ",");
        count++;
    }

    data_addr->sin_port = htons(port);

    if (inet_pton(AF_INET, serv_addr_str, &(data_addr->sin_addr)) <= 0)
        fatal_err("Invalid address!\n"); 

    if (connect(*data_fd, (struct sockaddr *) data_addr, sizeof(*data_addr)) != 0)
        fatal_err("Unable to setup data connection!");

}

void list(int ctrl_fd, int data_fd)
{
    char data_buf[10*BUF_SIZE];

    send_string(ctrl_fd, "LIST\r\n");

    if(recv(data_fd, data_buf, 10*BUF_SIZE, 0) <= 0)
        fatal_err("Some problems here!");
    else
        printf("%s\n", data_buf);

    // i don't want to analyse these 2 messages
    if (recv(ctrl_fd, data_buf, 10*BUF_SIZE, 0) <= 0)
        fatal_err("Some problems here x2!");
    if (recv(ctrl_fd, data_buf, 10*BUF_SIZE, 0) <= 0)
        fatal_err("Some problems here x3!");

}

void print_menu()
{
    printf("Choose action: \n"
           "\t[0]: Exit\n"
           "\t[1]: List current directory\n"
           "\t[2]: Get ftp-session and server info\n");
}

void session_info(int socket)
{
    char buf[BUF_SIZE];
    int n;

    send_string(socket, "STAT\r\n");
    
    do {
        n = recv(socket, buf, BUF_SIZE - 1, 0);
        buf[n] = 0;
        printf("Stats: %s\n", buf);

    } while (strstr(buf, "End") == NULL);
    
}

