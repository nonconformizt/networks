#if !defined(STRUCT)
#define STRUCT


#define BUF_SIZE 4096
#define SITE_ROOT "/var/www/mywww"
#define DEFAULT_DOC "index.html"
#define ERR_EMULATION 1

enum Method 
{
    GET, 

    // THESE ARE STILL NOT SUPPORTED
    POST,
    HEAD,
    PUT,
    DELETE,
    OPTIONS,
    TRACE,
    CONNECT
};

struct Request
{
    unsigned short errno;
    unsigned short httpver; // version: 1.1 as 11, 3.0 as 30, etc.
    enum Method method;
    char * path;
};


#endif // STRUCT
