#include <unistd.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <arpa/inet.h>


int send_string(int socket, const char *sString);

// Send ascii string to server
int send_string(int socket, const char *sString)
{
    return send(socket, sString, strlen(sString), 0);
}
