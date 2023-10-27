#include <unistd.h> 
#include <sys/socket.h> 
#include <netinet/in.h> 
#include <string.h> 
#include <arpa/inet.h>
#include <stdlib.h>
#include <sys/time.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <asm/socket.h>

#include "../lib/types.h"
#include "../lib/colors.h"

int makeBroadcast(struct sockaddr_in* addrOut){
    int broadcastFd = socket(AF_INET, SOCK_DGRAM, 0);
    if (broadcastFd < 0) return broadcastFd;

    struct sockaddr_in addr;
    int broadcast = 1;
    int reuseport = 1;
    setsockopt(broadcastFd, SOL_SOCKET, SO_BROADCAST, &broadcast, sizeof(broadcast));
    setsockopt(broadcastFd, SOL_SOCKET, SO_REUSEPORT, &reuseport, sizeof(reuseport));

    addr.sin_family = AF_INET;
    addr.sin_port = htons(1234);
    addr.sin_addr.s_addr = inet_addr("192.168.1.255");
    *addrOut = addr;
    return broadcastFd;
}

int makeTCP(struct sockaddr_in* addrOut){
    int tcpFd;
    tcpFd = socket(AF_INET, SOCK_DGRAM, 0);
    struct sockaddr_in addr;
    int reuseport = 1;
    setsockopt(tcpFd, SOL_SOCKET, SO_REUSEPORT, &reuseport, sizeof(reuseport));

    addr.sin_family = AF_INET;
    addr.sin_port = htons(1234);
    addr.sin_addr.s_addr = inet_addr("192.168.1.255");
    *addrOut = addr;
    return tcpFd;
}

void handleIncomingBC(char* buffer, char* identifier){
    if (strncmp(buffer, identifier, ID_SIZE) == 0) 
        return;
    
    if (strncmp(&buffer[ID_SIZE], "start working", strlen("start working")) == 0) {
        write(1, ANSI_YEL, sizeof(ANSI_YEL) - 1);
        write(1, buffer, ID_SIZE);
        write(1, ANSI_RST, sizeof(ANSI_RST) - 1);
        write(1, " restuarant" ANSI_GRN " opened\n" ANSI_RST, 
                strlen(" restuarant opened\n") + sizeof(ANSI_YEL) + sizeof(ANSI_RST) - 2);

        return;
    }
    else if (strncmp(&buffer[ID_SIZE], "break", strlen("break")) == 0) {
        write(1, ANSI_YEL, sizeof(ANSI_YEL) - 1);
        write(1, buffer, ID_SIZE);
        write(1, ANSI_RST, sizeof(ANSI_RST) - 1);
        write(1, " restuarant" ANSI_RED " closed" ANSI_RST "\n", 
                strlen(" restuarant closed\n") + sizeof(ANSI_RED) + sizeof(ANSI_RST) - 2);

        return;
    }
    write(0, buffer, BUFFER_SIZE);
}

int main(int argc, char const *argv[]) {
    int tcpSock, bcSock, broadcast = 1, opt = 1;
    char buffer[1024] = {0};
    struct sockaddr_in bcAddress, tcpAddress;
    fd_set readfds;
        char identifier[100];

    sprintf(identifier, "%d", getpid());
    char username[100];
    memcpy(username, "cust1", strlen("cust1"));
    // memcpy(buffer, identifier, strlen(identifier));

    tcpSock = makeTCP(&tcpAddress);
    bcSock = makeBroadcast(&bcAddress);
    // setsockopt(sock, SOL_SOCKET, SO_BROADCAST, &broadcast, sizeof(broadcast));
    // setsockopt(tcpSock, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt));

    bind(bcSock, (struct sockaddr *)&bcAddress, sizeof(bcAddress));
    while (1) {
        FD_ZERO(&readfds);
        FD_SET(tcpSock, &readfds);
        FD_SET(bcSock, &readfds);
        FD_SET(0, &readfds);

        select(bcSock + 1, &readfds, NULL, NULL, NULL);


        if (FD_ISSET(0, &readfds)) {
            memset(buffer, 0, 1024);
            read(0, buffer+strlen(username), 1024-strlen(username));
            memcpy(buffer, username, strlen(username));

                sendto(bcSock, buffer, strlen(buffer), 0,(struct sockaddr *)&bcAddress, sizeof(bcAddress));
        }

        if (FD_ISSET(bcSock, &readfds)) {
            memset(buffer, 0, 1024);
            recv(bcSock, buffer, 1024, 0);
            handleIncomingBC(buffer, username);
        }
    }
    // write(0, "Enter username: ", sizeof("Enter username: "));
    // memcpy(buffer+strlen(identifier), "hello", strlen("hello"));
    // read(0, username, 100);
    // memcpy(buffer+strlen(identifier)+strlen("hello"), username, strlen(username));
    
    // sendto(tcpSock, buffer, strlen(buffer), 0,(struct sockaddr *)&bcAddress, sizeof(bcAddress));

    return 0;
}