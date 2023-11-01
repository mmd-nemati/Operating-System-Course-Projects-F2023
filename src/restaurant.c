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
#include "../lib/logger.h"
#include "../lib/colors.h"
#include "../lib/alarm.h"
#include "../lib/tcp.h"
#include "../lib/user.h"
#include "../lib/udp.h"
#include "../lib/json.h"
#include "../lib/utils.h"

char identifier[100];
Menu* menu;
RestaurantState state = CLOSED;
SupplierInfo suppliers[MAX_SUPPLIER];
int suppliersCount = 0;

OrderInfo orders[MAX_ORDER];
int ordersCount = 0;

SaleInfo sales[MAX_SALE];
int salesCount = 0;

Ingredient ingredients[MAX_INGREDIENT];
int ingredsCount = 0;

void sendHelloRestaurant(int fd, struct sockaddr_in bcAddress, unsigned short tcpPort, char* buffer, char* username) {
    memset(buffer, '\0', BUFFER_SIZE);
    strncpy(buffer, identifier, ID_SIZE);
    sprintf(&buffer[ID_SIZE], "hello restaurant-%s-%hu", username, tcpPort);
    sendto(fd, buffer, BUFFER_SIZE, 0,(struct sockaddr *)&bcAddress, sizeof(bcAddress));
}

int isSupplierUnique(const char *username) {
    for (int i = 0; i < suppliersCount; i++) {
        if (strcmp(suppliers[i].username, username) == 0) {
            return 0;  // Username is not unique
        }
    }
    return 1;  // Username is unique
}

void addSupplier(const char *username, unsigned short port) {
    SupplierInfo* newSupplier = &suppliers[suppliersCount];
    suppliersCount++;
    memset(newSupplier->username, '\0', ID_SIZE);
    strcpy(newSupplier->username, username);
    newSupplier->port = port;
}

void addOrder(const char *username, unsigned short port, const char *food) {
    OrderInfo* newOrder = &orders[ordersCount];
    ordersCount++;
    memset(newOrder->username, '\0', ID_SIZE);
    strcpy(newOrder->username, username);
    newOrder->port = port;
    memset(newOrder->food, '\0', MAX_FOOD_NAME);
    strcpy(newOrder->food, food);
}

void addSale(const char *username, const char *food, OrderResult result) {
    SaleInfo* newSale = &sales[salesCount];
    salesCount++;
    memset(newSale->username, '\0', ID_SIZE);
    strcpy(newSale->username, username);
    memset(newSale->food, '\0', MAX_FOOD_NAME);
    strcpy(newSale->food, food);
    newSale->result = result;
}

CLIResult handleCLI(char *username){
    CLIResult answer;
    answer.result = 1;
    memset(answer.buffer, '\0', BUFFER_SIZE);
    read(0, &answer.buffer[ID_SIZE], BUFFER_SIZE - ID_SIZE);
    if (strncmp(&answer.buffer[ID_SIZE], "start working", strlen("start working")) == 0) {
        if (state == OPEN) {
            logTerminalWarning("Restaurant is already open");
            // TODO log file
            answer.result = 0;
            return answer;
        }
        // sprintf(answer.buffer, "%sstart working-%s-",identifier, username);
        sprintf(&answer.buffer[ID_SIZE], "start working-%s-", username);
        logTerminalInfo("Restaurant is now open");
        state = OPEN;
    }
    if (state == CLOSED) {
        logTerminalError("Restaurant is closed");
        answer.result = 0;
        return answer;
    }
    if (strncmp(&answer.buffer[ID_SIZE], "break", strlen("break")) == 0) {
        if (state == CLOSED) {
            logTerminalWarning("Restaurant is already closed");
            // TODO log file
            answer.result = 0;
            return answer;
        }
        logTerminalInfo("Restaurant is now closed");
        state = CLOSED;
    }
    else if (strncmp(&answer.buffer[ID_SIZE], "show suppliers", strlen("show suppliers")) == 0) {
        answer.result = 0;
        printSuppliers(suppliers, suppliersCount);
        // TODO log file
    }
    else if (strncmp(&answer.buffer[ID_SIZE], "show requests list", strlen("show requests list")) == 0) {
        printOrders(orders, ordersCount);
        // TODO log file
    }
    else if (strncmp(&answer.buffer[ID_SIZE], "show sales history", strlen("show sales history")) == 0) {
        printSales(sales, salesCount);
        // TODO log file
    }
    else if (strncmp(&answer.buffer[ID_SIZE], "show ingredients", strlen("show ingredients")) == 0) {
        printIngredients(ingredients, ingredsCount);
        // TODO log file
    }
    else if (strncmp(&answer.buffer[ID_SIZE], "show recipes", strlen("show recipes")) == 0) {
        printRecipes(menu);
        // TODO log file
    }
    return answer;
    // TODO log file
}
void handleIncomingBC(int fd, struct sockaddr_in bcAddress, unsigned short tcpPort, char* buffer, char* username){
    if (strncmp(buffer, identifier, strlen(identifier)) == 0) // check self broadcasting
        return; 

    if (strncmp(&buffer[ID_SIZE], "username check", strlen("username check")) == 0) {
        handleUsernameCheck(buffer, username);
        return;
    }
    if (state == OPEN && strncmp(&buffer[ID_SIZE], "hello supplier", strlen("hello supplier")) == 0) {
            strtok(&buffer[ID_SIZE], "-");
            char* username = strtok(NULL, "-"); // username
            char* port = strtok(NULL, "-");
            if (!isSupplierUnique(username))
                return;
            
            addSupplier(username, atoi(port));
            return;
    }
    else if (strncmp(&buffer[ID_SIZE], "hello customer", strlen("hello customer")) == 0) {
        sendHelloRestaurant(fd, bcAddress, tcpPort, buffer, username);;
        return;
    }
    write(0, buffer, BUFFER_SIZE);
}

int main(int argc, char const *argv[]) {
    CLIResult ans;
    menu = readJson("recipes.json");
    // printf("sss %s\n", menu->foods[0]->ingredients[0].name);
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
    sendHelloRestaurant(bcSock, bcAddress, htons(tcpAddress.sin_port), buffer, username);
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
            handleIncomingBC(bcSock, bcAddress, htons(tcpAddress.sin_port),buffer, username);
        }
    }
    // write(0, "Enter username: ", sizeof("Enter username: "));
    // memcpy(buffer+strlen(identifier), "hello", strlen("hello"));
    // read(0, username, 100);
    // memcpy(buffer+strlen(identifier)+strlen("hello"), username, strlen(username));
    
    // sendto(tcpSock, buffer, strlen(buffer), 0,(struct sockaddr *)&bcAddress, sizeof(bcAddress));

    return 0;
}