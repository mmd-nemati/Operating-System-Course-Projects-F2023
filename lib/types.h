#ifndef __TYPES_H__
#define __TYPES_H__

#include <netinet/in.h>
#include <sys/select.h>

#define BUF_NAME  32
#define BUF_PNAME 64
#define ID_SIZE   100
#define BUF_CLI   128
#define BUFFER_SIZE   1024
#define BCAST_IP  "192.168.1.255"
#define TIMEOUT   60
#define TOKEN_DEILIMITER "\n"

typedef struct {
    char id[ID_SIZE];
    char command[BUF_CLI];
    unsigned short port;
    int socket;
} BroadcastData;

typedef struct {
    int max;
    fd_set master;
    fd_set working;
} FdSet;

typedef struct {
    int fd;
    struct sockaddr_in addr;
} BroadcastInfo;

typedef enum {
    OPEN = 0,
    CLOSED = 1
} RestaurantState;

typedef struct {
    int result;
    char buffer[BUFFER_SIZE];
} CLIResult;
#endif // __TYPES_H__