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
#include "../lib/udp.h"

char identifier[100];

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

    tcpSock = makeTCP(&tcpAddress);
    bcSock = makeBroadcast(&bcAddress, 1234);

    char username[100];
    getUsername(username);
    while(sendUsernameCheck(bcSock, tcpSock, bcAddress, username, identifier, htons(tcpAddress.sin_port)))
        getUsername(username);

    write(0, ANSI_GRN "\tWelcome!\n\n" ANSI_RST, strlen("\tWelcome!\n\n") + ANSI_LEN);

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