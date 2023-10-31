#include "../lib/udp.h"

int makeBroadcast(struct sockaddr_in* addrOut, unsigned short port) {
    int broadcastFd = socket(AF_INET, SOCK_DGRAM, 0);
    if (broadcastFd < 0) return broadcastFd;
    
    struct sockaddr_in addr;
    int broadcast = 1;
    int reuseport = 1;
    setsockopt(broadcastFd, SOL_SOCKET, SO_BROADCAST, &broadcast, sizeof(broadcast));
    setsockopt(broadcastFd, SOL_SOCKET, SO_REUSEPORT, &reuseport, sizeof(reuseport));

    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = inet_addr("255.255.255.255"); 
    *addrOut = addr;
    bind(broadcastFd, (struct sockaddr *)&addr, sizeof(addr));
    
    return broadcastFd;
}