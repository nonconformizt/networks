#include "header.h"
#include "functions.c"


int main (int argc, char * argv[])
{
    struct in_addr addr, mask;
    int port_lo, port_hi;

    parse_args(argc, argv, &addr, &mask, &port_lo, &port_hi);

    int socket_fd = socket(AF_INET, SOCK_RAW, IPPROTO_TCP);
    if (socket_fd < 0)
        fatal_err("Error creating socket!");

    // Set IP_HDRINCL socket option to tell the kernel 
    // that headers are included in the packet
    int oneVal = 1;
    if (setsockopt(socket_fd, IPPROTO_IP, IP_HDRINCL, &oneVal, sizeof(oneVal)) < 0)
        fatal_err("Error setting IP_HDRINCL!");


    scan_host(addr, port_lo, port_hi, socket_fd);
}

