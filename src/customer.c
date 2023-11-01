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

char identifier[100];
Menu* menu;
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
CLIResult handleCLI(char *username){
    CLIResult answer;
    answer.result = 1;
    memset(answer.buffer, '\0', BUFFER_SIZE);
    read(0, &answer.buffer[ID_SIZE], BUFFER_SIZE - ID_SIZE);
    if (strncmp(&answer.buffer[ID_SIZE], "show restaurants", strlen("show restaurants")) == 0) {
        answer.result = 0;
        printRestaurants();
        // return answer;
    }
    else if (strncmp(&answer.buffer[ID_SIZE], "show menu", strlen("show menu")) == 0) {
        answer.result = 0;
        printMenu(menu);
        // return answer;
    }
    return answer;
    // TODO log file
}
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
    if (strncmp(&buffer[ID_SIZE], "hello restaurant", strlen("hello restaurant")) == 0) {
        strtok(&buffer[ID_SIZE], "-");
        char* username = strtok(NULL, "-"); // username
        char* port = strtok(NULL, "-");
        if (!isRestaurantUnique(username))
            return;
        
        addRestaurant(username, atoi(port));
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
    CLIResult ans;
    menu = readJson("recipes.json");
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
    sendHelloCustomer(bcSock, bcAddress, htons(tcpAddress.sin_port), buffer, username);
        int maxSock = (bcSock > tcpSock) ? bcSock : tcpSock;
    while (1) {
        FD_ZERO(&readfds);
        FD_SET(tcpSock, &readfds);
        FD_SET(bcSock, &readfds);
        FD_SET(0, &readfds);
        select(maxSock + 1, &readfds, NULL, NULL, NULL);

        if (FD_ISSET(0, &readfds)) {
            memset(buffer, '\0', 1024);
            ans = handleCLI(username);
            if (ans.result == 1) {
                memcpy(ans.buffer, identifier, ID_SIZE);
                sendto(bcSock, ans.buffer, BUFFER_SIZE, 0,(struct sockaddr *)&bcAddress, sizeof(bcAddress));
            }
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