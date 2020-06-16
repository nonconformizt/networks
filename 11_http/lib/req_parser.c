#include <unistd.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include "./headers.h"
#include "./response.c"
#include "./str_utils.c"


void handle_connection(int newsock_fd);
struct Request * parse_request(char * request);
void parse_first_str(struct Request * req, char * str);
void print_request(struct Request * req);


void handle_connection(int newsock_fd)
{
    struct Request * request;
    char req_str[BUF_SIZE];
    char *req_copy, *response;
    int n, resp_length;

    // TODO: read as many times as needed to read all (while read != 0)
    n = read(newsock_fd, req_str, BUF_SIZE-1);
    if (n < 0) 
    {
        fprintf(stderr, "Can't create socket!\n");
        exit(1);
    } else if (n == 0)
    {
        // Do I need to throw error here?
        fprintf(stderr, "No request!\n");
        exit(1);
    }
    req_str[n + 1] = 0;
    req_copy = (char *)malloc(BUF_SIZE);
    strcpy(req_copy, req_str);

    request = parse_request(req_copy);
    print_request(request);


    response = prepare_response(request, &resp_length);

    // fprintf(stdout, "Requested resourse: %s\n", request->path);
    fprintf(stdout, "====> Response: %s\n", response);

    send(newsock_fd, response, resp_length, 0);
    // send_string(newsock_fd, response);

}


struct Request * parse_request(char * req_str)
{
    struct Request * req = malloc(sizeof(struct Request));

    // char *str = strtok(req_str, "\r\n");
    // if (str == NULL) // assume request as one string 
    // {
        parse_first_str(req, req_str);
    // }
    // else 
    // {
    //     int counter = 0;
    //     while (str != NULL)
    //     {
    //         if (counter == 0)
    //             parse_first_str(&req, str);

    //         printf("%s\n", str);
    //         str = strtok(NULL, "\r\n");
    //         counter++;
    //     }
    // }

    return req;
}


void parse_first_str(struct Request * req, char * req_str)
{
    int pathlen;
    char * path;

    printf("Parsing first string... %s\n", req_str);

    char * str = strtok(req_str, " ");

    if (str == NULL) // no spaces in string => something is wrong
    {
        req->errno = 400; // bad request
        return;
    }

    // else keep parsing
    int counter = 0;
    while (str != NULL)
    {
        switch (counter)
        {
            case 0: 
                // METHOD
                if (strcmp(str, "GET") == 0) {
                    req->method = GET;
                } else if (strcmp(str, "POST") == 0) {
                    req->method = POST;
                } else if (strcmp(str, "HEAD") == 0) {
                    req->method = HEAD;
                } else if (strcmp(str, "PUT") == 0) {
                    req->method = PUT;
                } else if (strcmp(str, "DELETE") == 0) {
                    req->method = DELETE;
                } else if (strcmp(str, "OPTIONS") == 0) {
                    req->method = OPTIONS;
                } else if (strcmp(str, "TRACE") == 0) {
                    req->method = TRACE;
                } else if (strcmp(str, "CONNECT") == 0) {
                    req->method = CONNECT;
                } else {
                    req->errno = 405; // method not allowed
                    return;
                }
            break;
            case 1:
                // PATH
                if (str[strlen(str) - 1] == '/') 
                { // folder
                    pathlen = strlen(SITE_ROOT) + strlen(str) + strlen(DEFAULT_DOC) + 1;
                    path = (char*) malloc(pathlen * sizeof(char));
                    sprintf(path, "%s%s%s", SITE_ROOT, str, DEFAULT_DOC);
                }
                else 
                {
                    pathlen = strlen(SITE_ROOT) + strlen(str) + 1;
                    path = (char*) malloc(pathlen * sizeof(char));
                    sprintf(path, "%s%s", SITE_ROOT, str);
                }
                req->path = path;
            break;
            case 2: 
                if (strncmp(str, "HTTP/x.x", 5) != 0 || strlen(str) < 8) 
                {
                    req->errno = 505; // HTTP version not supported
                    return;
                }
                // convert http version to number
                req->httpver = 10*(str[5] - '0') + (str[7] - '0');
            break;
            default: 
                return;
            break;
        }
        str = strtok(NULL, " ");
        counter++;
    }

}


void print_request(struct Request * req)
{
    if (req != NULL)
        printf("Request: \nVersion = %d\nMethod = %d\nPath = %s\nErr = %d \n", req->httpver, req->method, req->path, req->errno);
}