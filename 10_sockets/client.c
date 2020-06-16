#include <unistd.h> 
#include <sys/socket.h> 
#include <stdlib.h> 
#include <arpa/inet.h>
#include <stdio.h> 
#include <stdio_ext.h>
#include "str_utils.c"

// Send ascii string to server
int send_string(int socket, const char *sString)
{
    return send(socket, sString, strlen(sString), 0);
}

void clear_buf(char * str, int len)
{
    for (; --len >= 0;) str[len] = 0;
}

int main(void)
{
    int socket_fd, serv_port;
    struct sockaddr_in serv_addr;
    char ser_addr_str[128];
    char response[1024] = {0};
    char buf[1024] = {0};
    char c;
    int terminate = 0;

    if ((socket_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        fprintf(stderr, "Can't create socket\n");
        exit(1);
    }

    // get server address from user
    printf("Specify server ip: ");
    fgets(ser_addr_str, 128, stdin);
    ser_addr_str[strlen(ser_addr_str) - 1] = '\0';

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    // convert IPv4 and IPv6 addresses from text to binary form 
    if(inet_pton(AF_INET, ser_addr_str, &serv_addr.sin_addr) <= 0)  
    { 
        fprintf(stderr, "\n[Error]: Invalid address!\n"); 
        return -1; 
    } 

    printf("IP ok\nSpecify server port: ");
    scanf("%d", &serv_port);
    serv_addr.sin_port = htons(serv_port);


    if (connect(socket_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) 
    { 
        fprintf(stderr, "\n[Error]: Connection Failed!\n"); 
        return -1; 
    }
    else  {
        printf("Connected!\n");
        printf("Insert your commands or type \"exit\" to terminate\n");
    }


    do
    {
        read(socket_fd, response, 1024);
        printf("%s\n", response);
        clear_buf(response, 1024);

        printf("> ");
        __fpurge(stdin);
        fgets(buf, 1024, stdin);
        if (strcmp(buf, "exit") == 0)
            terminate = 1;
        send_string(socket_fd, buf);
        clear_buf(buf, 1024);

    } while (!terminate);


}

