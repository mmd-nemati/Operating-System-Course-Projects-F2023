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
#include "../lib/utils.h"

char identifier[100];
char username[100];

int reqPending = 0;
int restSock;
void sendHelloSupplier(int fd, struct sockaddr_in bcAddress, unsigned short tcpPort, char* buffer){
    memset(buffer, '\0', BUFFER_SIZE);
    strncpy(buffer, identifier, ID_SIZE);
    sprintf(&buffer[ID_SIZE], "hello supplier-%s-%hu", username, tcpPort);
    sendto(fd, buffer, BUFFER_SIZE, 0,(struct sockaddr *)&bcAddress, sizeof(bcAddress));
}

CLIResult handleCLI(){
    CLIResult answer;
    answer.result = 0;
    memset(answer.buffer, '\0', BUFFER_SIZE);
    read(0, &answer.buffer[ID_SIZE], BUFFER_SIZE - ID_SIZE);
    if (strncmp(&answer.buffer[ID_SIZE], "answer request", strlen("answer request")) == 0) {
        if (!reqPending) {
            logTerminalError("No pending request\n");
            return answer;
        }

        char message[MESSAGE_SIZE];
        char inp[BUFFER_SIZE];
        memset(message, '\0', MESSAGE_SIZE);
        memcpy(message, identifier, ID_SIZE);
        memset(inp, '\0', BUFFER_SIZE);
        write(1, "your answer (accept/reject): ", strlen("your answer (accept/reject): "));
        read(0, inp, BUFFER_SIZE);
        if (strncmp(inp, "accept", strlen("accept")) == 0) {
            write(1, "Accepted request!\n\n", strlen("Accepted request!\n\n"));
            sprintf(&message[ID_SIZE], "accept%s Supplier accepted!\n", username);
        } else {
            write(1, "Rejected request!\n\n", strlen("Rejected request!\n\n"));
            sprintf(&message[ID_SIZE], "reject%s Supplier denied!\n", username);
        }
        send(restSock, message, MESSAGE_SIZE, 0);
        logFile("'request ingredient' response sent.", username);
        reqPending = 0;
        answer.result = 0;
    }
        return answer;
    // TODO log file
}

void handleUDP(int fd, struct sockaddr_in bcAddress, unsigned short tcpPort, char* buffer){
    if (strncmp(buffer, identifier, ID_SIZE) == 0) 
        return;

    if (strncmp(&buffer[ID_SIZE], "username check", strlen("username check")) == 0) {
        handleUsernameCheck(buffer, username);
        logFile("'username check' command completed", username);
        return;
    }

    if (strncmp(&buffer[ID_SIZE], "start working", strlen("start working")) == 0) {
        sendHelloSupplier(fd, bcAddress, tcpPort, buffer);
        logFile("Sent hello supplier.", username);
        return;
    }
}

void handleTCP(char *buffer) {
    if (strncmp(&buffer[ID_SIZE], "request ingredient", strlen("request ingredient")) == 0) {
        if (reqPending) {
            int orgRest = restSock;
            unsigned short port = extractPort(buffer, 5);
            restSock = cnctServer(port);
            logFile("Connected to new restaurant", username);
            memset(buffer, '\0', BUFFER_SIZE);
            // strcpy(&buffer[ID_SIZE], "reject%s Supplier is busy!", username);
            sprintf(&buffer[ID_SIZE], "reject%s Supplier is busy!\n", username);
            send(restSock, buffer, BUFFER_SIZE, 0);
            restSock = orgRest;
            logTerminalInfo("New ingredient request received and\n\tis rejected because you have a pending request.\n");
            logFile("'request ingredient' request rejected.[busy]", username);
            return;
        }
        unsigned short port = extractPort(buffer, 5);
        restSock = cnctServer(port);
        logFile("Connected to restaurant", username);
        reqPending = 1;
        logTerminalInfo("\tNew ingredient request received.\n");
        logFile("'request ingredient' request pending.", username);
    }
    else if (strncmp(buffer, "time out", strlen("time out")) == 0) {
        reqPending = 0;
        restSock = -1;
        logTerminalWarning("Ingredient request expired.\n");
    }


}

void initSupp() {
    memset(identifier, '\0', sizeof(identifier));
    sprintf(identifier, "%d", getpid());
    memset(username, '\0', sizeof(identifier));
    logFile("initialized", username);
}

int main(int argc, char const *argv[]) {
    CLIResult ans;
    initSupp();
    int tcpSock, udpSock, maxSock;
    char buffer[BUFFER_SIZE] = {0};
    struct sockaddr_in bcAddress, tcpAddress;
    fd_set workingSet, masterSet;

    tcpSock = makeTCP(&tcpAddress);
    udpSock = makeUDP(&bcAddress, (unsigned short)atoi(argv[1]));

    getUsername(username);
    while(sendUsernameCheck(udpSock, tcpSock, bcAddress, username, identifier, htons(tcpAddress.sin_port)))
        getUsername(username);
    write(0, ANSI_GRN "\tWelcome!\n\n" ANSI_RST, strlen("\tWelcome!\n\n") + ANSI_LEN);
    logFile("Logged in.", username);

    sendHelloSupplier(udpSock, bcAddress, htons(tcpAddress.sin_port), buffer);
    logFile("Sent hello supplier.", username);
    
    FD_ZERO(&masterSet);
    FD_SET(STDIN_FILENO, &masterSet);
    FD_SET(tcpSock, &masterSet);
    FD_SET(udpSock, &masterSet);
    maxSock = (udpSock > tcpSock) ? udpSock : tcpSock;
    while (1) {
        workingSet = masterSet;
        select(maxSock + 1, &workingSet, NULL, NULL, NULL);

        if (FD_ISSET(0, &workingSet)) {
            ans = handleCLI(username);
            if (ans.result == 1) {
                memset(buffer, 0, 1024);
                read(0, buffer+strlen(username), 1024-strlen(username));
                memcpy(buffer, username, strlen(username));
                sendto(udpSock, buffer, strlen(buffer), 0,(struct sockaddr *)&bcAddress, sizeof(bcAddress));

            }
        }
        else if (FD_ISSET(udpSock, &workingSet)) {
            memset(buffer, 0, 1024);
            recv(udpSock, buffer, 1024, 0);
            handleUDP(udpSock, bcAddress, htons(tcpAddress.sin_port), buffer);
        }
        else if (FD_ISSET(tcpSock, &workingSet)) { // new clinet
            int newSocket = accClient(tcpSock);
            FD_SET(newSocket, &masterSet);
            maxSock = (newSocket > maxSock) ? newSocket : maxSock;
        }
        else {
            for (int i = 3; i <= maxSock; i++) { // tcp get message
                if (FD_ISSET(i, &workingSet)) {
                    int bytes_received;
                    memset(buffer, '\0', BUFFER_SIZE);
                    bytes_received = recv(i, buffer, BUFFER_SIZE, 0);
                    if (bytes_received == 0) {  // EOF
                        char msg[MESSAGE_SIZE];  // TODO LOG
                        close(i);
                        FD_CLR(i, &masterSet);
                        continue;
                    }
                    handleTCP(buffer);
                }
            }
        }
    }
    return 0;
}