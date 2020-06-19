#include <unistd.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <sys/utsname.h>
#include "./headers.h"

char * prepare_response(struct Request * request, int * resp_len);
char * error_response(u_short errno);
char * get_content_type_h(const char * path);
char * get_content_len_h(int len);
char * get_server_h();
char * get_date_h();
const char * get_filename_ext(const char * filename);



char * prepare_response(struct Request * request, int * resp_len)
{
    int fsize;
    FILE * fp;
    *resp_len = BUF_SIZE;

    if (request->errno != 0) 
    {
        return error_response(request->errno);
    }
    if (request->method != GET)
    {
        request->errno = 418; // ¯\_(ツ)_/¯
        return error_response(418);
    }
    #ifdef ERR_EMULATION
        if (strstr(request->path, "error") != NULL)
        {
            request->errno = 500; // internal server error
            return error_response(500);
        }
    #endif
    
    char * path_cpy = malloc(strlen(request->path));
    strcpy(path_cpy, request->path);
    if ((fp = fopen(path_cpy, "rb")) == NULL)
    {
        perror("File open error: ");
        request->errno = 404;
        return error_response(404);
    }

    // calculate file size
    fseek(fp, 0L, SEEK_END);
    fsize = ftell(fp);
    rewind(fp);

    char * response = malloc(5 * BUF_SIZE + fsize);
    *resp_len = 5 * BUF_SIZE + fsize;
    
    char * server_h = get_server_h();
    char * contlen_h = get_content_len_h(fsize);
    char * conttype_h = get_content_type_h(request->path);
    char * date_h = get_date_h();

    sprintf(response, "HTTP/1.1  200 OK\r\n%s\r\n%s\r\n%s\r\n%s\r\n\r\n", server_h, contlen_h, conttype_h, date_h);

    // load file content to buffer
    fread(response + strlen(response), 1, fsize, fp);
    fclose(fp);

    return response;
}

char * error_response(u_short errno)
{
    int fsize = 0;
    FILE * fp;
    char * response;
    char * default_f = malloc(BUF_SIZE);

    sprintf(default_f, "%s/%d.html", SITE_ROOT, errno);

    if ((fp = fopen(default_f, "r")) != NULL)
    {
        fseek(fp, 0L, SEEK_END);
        fsize = ftell(fp);
        rewind(fp);
    }
    
    response = malloc(BUF_SIZE + fsize);
    sprintf(response, "HTTP/1.1  %d ERROR\r\n\r\n", (int) errno);

    if (fp != NULL)
    {
        fread(response + strlen(response), 1, fsize, fp);
        fclose(fp);
    }

    return response;
}

char * get_content_type_h(const char * path)
{
    // TODO: proper content-type generation
    char * header = malloc(BUF_SIZE);
    char * ext = (char *)get_filename_ext(path);

    if (strcmp(ext, "jpeg") == 0 ||
        strcmp(ext, "jpg") == 0 ||
        strcmp(ext, "png") == 0 ||
        strcmp(ext, "webp") == 0) {
        sprintf(header, "Content-Type: image/%s", ext);
    }
    else {
        sprintf(header, "Content-Type: text/%s", ext);
    }
    return header;
}

char * get_content_len_h(int len)
{
    char * header = malloc(BUF_SIZE);
    sprintf(header, "Content-Length: %d", len);
    return header;
}

char * get_server_h()
{
    struct utsname info;
    uname(&info);    
    char * header = malloc(BUF_SIZE);
    sprintf(header, "Server: %s %s", info.sysname, info.version);
    return header;
}

char * get_date_h()
{
    time_t rawtime;
    struct tm * info;
    time(&rawtime);
    info = gmtime(&rawtime);

    char * header = malloc(BUF_SIZE);
    strftime(header, BUF_SIZE, "Date: %a, %d %b %Y %H:%M:%S GMT", info);

    return header;
}

const char * get_filename_ext(const char * filename) 
{
    const char * dot = strrchr(filename, '.');
    if(!dot || dot == filename) return "";
    return dot + 1;
}

