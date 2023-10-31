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
#include "../lib/user.h"
#include "../lib/tcp.h"
#include "../lib/udp.h"

char identifier[100];

void handleIncomingBC(char* buffer, char* username){
    if (strncmp(buffer, identifier, ID_SIZE) == 0) 
        return;
    if (strncmp(&buffer[ID_SIZE], "username check", strlen("username check")) == 0) {
        handleUsernameCheck(buffer, username);
        return;
    }
    if (strncmp(&buffer[ID_SIZE], "start working", strlen("start working")) == 0) {
        strtok(&buffer[ID_SIZE], "-");
        char* restName = strtok(NULL, "-"); // restaurant name
        write(1, ANSI_YEL, sizeof(ANSI_YEL) - 1);
        write(1, restName, ID_SIZE);
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