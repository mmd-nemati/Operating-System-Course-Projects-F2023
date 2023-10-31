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
#include "../lib/tcp.h"
#include "../lib/user.h"

char identifier[100];

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
    addr.sin_addr.s_addr = inet_addr("255.255.255.255"); 
    *addrOut = addr;
    return broadcastFd;
}

// int makeTCP(struct sockaddr_in* addrOut){
//     int tcpFd;
//     tcpFd = socket(AF_INET, SOCK_STREAM, 0);
//     struct sockaddr_in addr;
//     int reuseport = 1;
//     setsockopt(tcpFd, SOL_SOCKET, SO_REUSEPORT, &reuseport, sizeof(reuseport));

//     addr.sin_family = AF_INET;
//     addr.sin_port = htons(1237);
//     addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK); 
//     *addrOut = addr;
//     bind(tcpFd, (struct sockaddr *)&addr, sizeof(addr));
    
//     listen(tcpFd, 4);
//     return tcpFd;
// }

void sendHelloSupplier(int fd, struct sockaddr_in addr, char* buffer, char* username){
    memset(buffer, '\0', BUFFER_SIZE);
    strncpy(buffer, identifier, ID_SIZE);
    sprintf(&buffer[ID_SIZE], "hello supplier-%s-%hu", username, htons(addr.sin_port));
    sendto(fd, buffer, BUFFER_SIZE, 0,(struct sockaddr *)&addr, sizeof(addr));
}

void handleIncomingBC(int fd, struct sockaddr_in addr, char* buffer, char* username){
    if (strncmp(buffer, identifier, ID_SIZE) == 0) 
        return;

    if (strncmp(&buffer[ID_SIZE], "username check", strlen("username check")) == 0) {
        handleUsernameCheck(buffer, username);
        return;
    }

    if (strncmp(&buffer[ID_SIZE], "start working", strlen("start working")) == 0) {
        sendHelloSupplier(fd, addr, buffer, username);
        return;
    }

    // write(0, buffer, BUFFER_SIZE);
}

int main(int argc, char const *argv[]) {
    int tcpSock, bcSock, broadcast = 1, opt = 1;
    char buffer[1024] = {0};
    struct sockaddr_in bcAddress, tcpAddress;
    fd_set readfds;
    memset(identifier, '\0', sizeof(identifier));
    sprintf(identifier, "%d", getpid());

    // sprintf(identifier, "%d", getpid());
    // char username[100];
    // memset(username, '\0', ID_SIZE);
    // // read(0, username, ID_SIZE);
    // read(0, username, ID_SIZE);
    //         // write(0, "anomoly", 7);

    // for (int i = 0; i < ID_SIZE; i++) 
    //     if (username[i] == '\n') {
    //         username[i] = '\0';
    //         break;
    //     }
    // strncpy(username, username, strlen(username) - 1);
    // memset(&username[strlen(username) - 1], '\0', ID_SIZE - strlen(username));

    // memcpy(username, "supp2", strlen("supp1"));
    // memcpy(buffer, identifier, strlen(identifier));

    tcpSock = makeTCP(&tcpAddress, 1237);
    bcSock = makeBroadcast(&bcAddress);
    // setsockopt(sock, SOL_SOCKET, SO_BROADCAST, &broadcast, sizeof(broadcast));
    // setsockopt(tcpSock, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt));
    bind(bcSock, (struct sockaddr *)&bcAddress, sizeof(bcAddress));

    char username[100];
    getUsername(username);
    while(sendUsernameCheck(bcSock, tcpSock, bcAddress, username, identifier, htons(tcpAddress.sin_port)))
        getUsername(username);

    write(0, ANSI_GRN "\tWelcome!\n\n", strlen("\tWelcome!\n\n") + strlen(ANSI_GRN) - 1);
    sendHelloSupplier(bcSock, bcAddress, buffer, username);
        int maxSock = (bcSock > tcpSock) ? bcSock : tcpSock;
    while (1) {
        FD_ZERO(&readfds);
        FD_SET(tcpSock, &readfds);
        FD_SET(bcSock, &readfds);
        FD_SET(0, &readfds);

        select(maxSock + 1, &readfds, NULL, NULL, NULL);


        if (FD_ISSET(0, &readfds)) {
            memset(buffer, 0, 1024);
            read(0, buffer+strlen(username), 1024-strlen(username));
            memcpy(buffer, username, strlen(username));
            sendto(bcSock, buffer, strlen(buffer), 0,(struct sockaddr *)&bcAddress, sizeof(bcAddress));
        }

        if (FD_ISSET(bcSock, &readfds)) {
            memset(buffer, 0, 1024);
            recv(bcSock, buffer, 1024, 0);
            handleIncomingBC(bcSock, bcAddress, buffer, username);
        }
    }
    // write(0, "Enter username: ", sizeof("Enter username: "));
    // memcpy(buffer+strlen(identifier), "hello", strlen("hello"));
    // read(0, username, 100);
    // memcpy(buffer+strlen(identifier)+strlen("hello"), username, strlen(username));
    
    // sendto(tcpSock, buffer, strlen(buffer), 0,(struct sockaddr *)&bcAddress, sizeof(bcAddress));

    return 0;
}