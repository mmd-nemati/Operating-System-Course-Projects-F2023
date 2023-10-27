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

int main(int argc, char const *argv[]) {
    int tcpSock, bcSock, broadcast = 1, opt = 1;
    char buffer[1024] = {0};
    struct sockaddr_in bcAddress, tcpAddress;
    fd_set readfds;
        char identifier[100];

    sprintf(identifier, "%d", getpid());
    char username[100];
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
        int maxSock = (tcpSock > bcSock) ? tcpSock : bcSock;
        select(maxSock + 1, &readfds, NULL, NULL, NULL);


        if (FD_ISSET(0, &readfds)) {
            memset(buffer, 0, 1024);
            read(0, buffer+strlen(identifier), 1024-strlen(identifier));
            memcpy(buffer, identifier, strlen(identifier));
            // if (strncmp(buffer+strlen(identifier), "start working", strlen("start working")) == 0) {
            //     sendto(bcSock, buffer, strlen(buffer), 0,(struct sockaddr *)&bcAddress, sizeof(bcAddress));
            // }
            // else
                sendto(bcSock, buffer, strlen(buffer), 0,(struct sockaddr *)&bcAddress, sizeof(bcAddress));
        }

        if (FD_ISSET(bcSock, &readfds)) {
            memset(buffer, 0, 1024);
            recv(bcSock, buffer, 1024, 0);
            if (strncmp(buffer, identifier,strlen(identifier)) != 0) {
                if (strncmp(buffer+strlen(identifier), "start working", strlen("start working")) == 0)
                    printf("heard starting to work:%c%c%c%c%c\n", buffer[0], buffer[1], buffer[2], buffer[3], 
                            buffer[4]);
                // printf("%s\n", buffer);

                // if (strncmp(buffer+strlen(identifier), "hello", 5) == 0){
                //     if ((strncmp(buffer+5, username, strlen(username)) == 0))
                //         sendto(tcpSock, "REJECT USERNAME", strlen("REJECT USERNAME"), 0,(struct sockaddr *)&bcAddress, sizeof(bcAddress));
                // }
                // if (strncmp(buffer, "REJECT USERNAME", strlen("REJECT USERNAME") == 0)) {
                //     printf("Rejected\n");
                //     exit(0);
                // }
            }
        }
    }
    // write(0, "Enter username: ", sizeof("Enter username: "));
    // memcpy(buffer+strlen(identifier), "hello", strlen("hello"));
    // read(0, username, 100);
    // memcpy(buffer+strlen(identifier)+strlen("hello"), username, strlen(username));
    
    // sendto(tcpSock, buffer, strlen(buffer), 0,(struct sockaddr *)&bcAddress, sizeof(bcAddress));

    return 0;
}