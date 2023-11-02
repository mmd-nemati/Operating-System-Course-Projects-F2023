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
#include "../lib/utils.h"
#include "../lib/json.h"
#include "../lib/logger.h"

char identifier[100];
char username[100];
Menu* menu;
int tcpSock, myTcpPort, timeOut = 0;

RestaurantInfo restaurants[100];
int restaurantsCount = 0;

void sendHelloCustomer(int fd, struct sockaddr_in bcAddress, unsigned short tcpPort, char* buffer, char* username){
    memset(buffer, '\0', BUFFER_SIZE);
    strncpy(buffer, identifier, ID_SIZE);
    sprintf(&buffer[ID_SIZE], "hello customer-%s-%hu", username, tcpPort);
    sendto(fd, buffer, BUFFER_SIZE, 0,(struct sockaddr *)&bcAddress, sizeof(bcAddress));
}
int isRestaurantUnique(const char *username) {
    for (int i = 0; i < restaurantsCount; i++) {
        if (strcmp(restaurants[i].username, username) == 0) {
            return 0;  // Username is not unique
        }
    }
    return 1;  // Username is unique
}

void addRestaurant(const char *username, unsigned short port) {
    RestaurantInfo* newRestaurant = &restaurants[restaurantsCount];
    restaurantsCount++;
    memset(newRestaurant->username, '\0', ID_SIZE);
    strcpy(newRestaurant->username, username);
    newRestaurant->port = port;
}
void printRestaurants() {
    if (restaurantsCount == 0) {
        printf("%sNo available restaurant%s\n", ANSI_RED, ANSI_RST);
        return;
    }
    printf("--------------------\n");
    printf("<username>::<port>\n");
    for (int i = 0; i < restaurantsCount; i++) 
        printf("%s%s%s::%s%hu%s\n", ANSI_YEL, restaurants[i].username, ANSI_RST, ANSI_GRN, restaurants[i].port, ANSI_RST);
    printf("--------------------\n");
}

void alarmHandlerCust(int sig) {
    timeOut = 1;
}

void handleFoodResponse(int restSock, OrderFoodData* food) {
    char buffer[BUFFER_SIZE];
    memset(buffer, '\0', BUFFER_SIZE);
    write(1, "waiting for the restaurant's response...\n", strlen("waiting for the restaurant's response...\n"));
    signal(SIGALRM, alarmHandlerCust);
    siginterrupt(SIGALRM, 1);
    struct termios originalTerminos = lockTerminal();
    alarm(40); // TODO change this
    int newSock = accClient(tcpSock);
    int bytesReceived = recv(newSock, buffer, BUFFER_SIZE, 0);
    if (timeOut) {
        memset(buffer, '\0', BUFFER_SIZE);
        sprintf(&buffer[ID_SIZE], "time out order-%s-", username);
        send(restSock, buffer, BUFFER_SIZE, 0);
        logTerminalError("Time out!");
        timeOut = 0;
        tcsetattr(STDIN_FILENO, TCSAFLUSH, &originalTerminos);
        logFile("'order food' request time out!", username);
        return;
    }
    alarm(0);
    if (bytesReceived == 0) 
        logTerminalWarning("restaurant closed!\n");
    if (strncmp(&buffer[ID_SIZE], "accept", strlen("accept")) == 0) {
        write(1, &buffer[ID_SIZE+6], BUFFER_SIZE - 106);

    }
    else if (strncmp(&buffer[ID_SIZE], "reject", strlen("reject")) == 0)
        write(1, &buffer[ID_SIZE+6], BUFFER_SIZE - 106);

    tcsetattr(STDIN_FILENO, TCSAFLUSH, &originalTerminos);
    logFile("'order food' response received", username);
}

void sendOrderFood() {
    char buffer[BUFFER_SIZE];
    memset(buffer, '\0', BUFFER_SIZE);
    OrderFoodData* data = getReqFoodData();
    memcpy(data->username, username, 100);
    int newSocket = cnctServer(data->port);
    sprintf(&buffer[ID_SIZE], "order food-%hu-%s-%s-%hu-",data->port, data->username, data->food, myTcpPort);
    memcpy(buffer, identifier, ID_SIZE);
    send(newSocket, buffer, BUFFER_SIZE, 0);
    logFile("'order food' request sent", username);
    handleFoodResponse(newSocket, data);
}

CLIResult handleCLI() {
    CLIResult answer;
    answer.result = 1;
    memset(answer.buffer, '\0', BUFFER_SIZE);
    read(0, &answer.buffer[ID_SIZE], BUFFER_SIZE - ID_SIZE);
    if (strncmp(&answer.buffer[ID_SIZE], "show restaurants", strlen("show restaurants")) == 0) {
        answer.result = 0;
        printRestaurants();
        logFile("'show restaurants' command completed", username);
        // return answer;
    }
    else if (strncmp(&answer.buffer[ID_SIZE], "show menu", strlen("show menu")) == 0) {
        answer.result = 0;
        printMenu(menu);
        logFile("'show menu' command completed", username);
    }
    else if (strncmp(&answer.buffer[ID_SIZE], "order food", strlen("order food")) == 0) {
        answer.result = 0; // TODO enum
        sendOrderFood();
        logFile("'order food' command completed", username);
    }
    return answer;
}
void handleUDP(char* buffer){
    if (strncmp(buffer, identifier, ID_SIZE) == 0)  // check self broadcasting
        return;

    if (strncmp(&buffer[ID_SIZE], "username check", strlen("username check")) == 0) {
        handleUsernameCheck(buffer, username);
        logFile("'username check' command completed", username);
    }
    else if (strncmp(&buffer[ID_SIZE], "start working", strlen("start working")) == 0) {
        strtok(&buffer[ID_SIZE], "-");
        char* restName = strtok(NULL, "-");
        write(1, ANSI_YEL, sizeof(ANSI_YEL) - 1);
        write(1, restName, ID_SIZE);
        write(1, ANSI_RST, sizeof(ANSI_RST) - 1);
        write(1, " restuarant" ANSI_GRN " opened\n" ANSI_RST, 
                strlen(" restuarant opened\n") + sizeof(ANSI_YEL) + sizeof(ANSI_RST) - 2);
    }
    else if (strncmp(&buffer[ID_SIZE], "hello restaurant", strlen("hello restaurant")) == 0) {
        strtok(&buffer[ID_SIZE], "-");
        char* username = strtok(NULL, "-");
        char* port = strtok(NULL, "-");
        if (!isRestaurantUnique(username))
            return;
        
        addRestaurant(username, atoi(port));
    }
    else if (strncmp(&buffer[ID_SIZE], "break", strlen("break")) == 0) {
        strtok(&buffer[ID_SIZE], "-");
        char* restName = strtok(NULL, "-");
        write(1, ANSI_YEL, sizeof(ANSI_YEL) - 1);
        write(1, restName, ID_SIZE);
        write(1, ANSI_RST, sizeof(ANSI_RST) - 1);
        write(1, " restuarant" ANSI_RED " closed" ANSI_RST "\n", 
                strlen(" restuarant closed\n") + sizeof(ANSI_RED) + sizeof(ANSI_RST) - 2);
    }
}

void initCust() {
    menu = readJson("recipes.json");
    memset(identifier, '\0', sizeof(identifier));
    sprintf(identifier, "%d", getpid());
    memset(username, '\0', sizeof(identifier));
    logFile("initialized", username);
}

int main(int argc, char const *argv[]) {
    CLIResult ans;
    initCust();
    int udpSock, maxSock;
    char buffer[1024] = {0};
    struct sockaddr_in bcAddress, tcpAddress;
    fd_set workingSet, masterSet;
    
    tcpSock = makeTCP(&tcpAddress);
    udpSock = makeUDP(&bcAddress, 1234);
    myTcpPort = htons(tcpAddress.sin_port);

    getUsername(username);
    while(sendUsernameCheck(udpSock, tcpSock, bcAddress, username, identifier, htons(tcpAddress.sin_port)))
        getUsername(username);
    write(0, ANSI_GRN "\tWelcome!\n\n" ANSI_RST, strlen("\tWelcome!\n\n") + ANSI_LEN);
    logFile("Logged in.", username);

    sendHelloCustomer(udpSock, bcAddress, htons(tcpAddress.sin_port), buffer, username);
    logFile("Sent hello customer.", username);

    FD_ZERO(&masterSet);
    FD_SET(STDIN_FILENO, &masterSet);
    FD_SET(tcpSock, &masterSet);
    FD_SET(udpSock, &masterSet);
    maxSock = (udpSock > tcpSock) ? udpSock : tcpSock;
    while (1) {
        workingSet = masterSet;
        select(maxSock + 1, &workingSet, NULL, NULL, NULL);

        if (FD_ISSET(0, &workingSet)) {
            memset(buffer, '\0', 1024);
            ans = handleCLI();
            if (ans.result == 1) {
                memcpy(ans.buffer, identifier, ID_SIZE);
                sendto(udpSock, ans.buffer, BUFFER_SIZE, 0,(struct sockaddr *)&bcAddress, sizeof(bcAddress));
            }
        }

        else if (FD_ISSET(udpSock, &workingSet)) {
            memset(buffer, 0, 1024);
            recv(udpSock, buffer, 1024, 0);
            handleUDP(buffer);
        }
    }
    return 0;
}