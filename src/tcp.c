#include "../lib/tcp.h"
#include <stdio.h>

int makeTCP(struct sockaddr_in* addrOut) {
    int tcpFd;
    tcpFd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in addr;
    int reuseport = 1;
    setsockopt(tcpFd, SOL_SOCKET, SO_REUSEPORT, &reuseport, sizeof(reuseport));

    addr.sin_family = AF_INET;
    addr.sin_port = htons(0);
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK); 
    bind(tcpFd, (struct sockaddr *)&addr, sizeof(addr));
    socklen_t addrLen = sizeof(addr);
    getsockname(tcpFd, (struct sockaddr *)&addr, &addrLen);
    listen(tcpFd, 4);
    *addrOut = addr;
    // printf("port %hu\n", htons(addr.sin_port));
    return tcpFd;
}

int accClient(int server_fd) {
    int client_fd;
    struct sockaddr_in client_address;
    int address_len = sizeof(client_address);
    
    client_fd = accept(server_fd, (struct sockaddr *)&client_address, (socklen_t*) &address_len);

    return client_fd;
}

int cnctServer(int port) {
    int fd;
    struct sockaddr_in server_address;
    
    fd = socket(AF_INET, SOCK_STREAM, 0);
    
    server_address.sin_family = AF_INET; 
    server_address.sin_port = htons(port); 
    server_address.sin_addr.s_addr = htonl(INADDR_LOOPBACK); 

    connect(fd, (struct sockaddr *)&server_address, sizeof(server_address)); // checking for errors

    return fd;
}
