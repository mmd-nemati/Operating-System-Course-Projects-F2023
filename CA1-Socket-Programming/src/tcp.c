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
    if(listen(tcpFd, 4) == -1) {
        write(2, "listen failed\n", strlen("listen failed\n"));
        exit(1);
    }
    *addrOut = addr;
    return tcpFd;
}

int accClient(int serverFd) {
    int clientFd;
    struct sockaddr_in addr;
    int addrLen = sizeof(addr);
    
    clientFd = accept(serverFd, (struct sockaddr *)&addr, (socklen_t*) &addrLen);

    return clientFd;
}

int cnctServer(int port) {
    int fd;
    struct sockaddr_in server_address;
    
    fd = socket(AF_INET, SOCK_STREAM, 0);
    
    server_address.sin_family = AF_INET; 
    server_address.sin_port = htons(port); 
    server_address.sin_addr.s_addr = htonl(INADDR_LOOPBACK); 

    if (connect(fd, (struct sockaddr *)&server_address, sizeof(server_address)) < 0)
        printf("Error in connecting to server\n");
    

    return fd;
}
