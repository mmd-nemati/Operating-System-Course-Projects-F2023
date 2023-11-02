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
int tcpSock;
unsigned short myTcpPort;
int timeOut = 0;

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
    for (int i = 0; i < suppliersCount; i++)
        if (strcmp(suppliers[i].username, username) == 0)
            return 0;  // Username is not unique

    return 1;  // Username is unique
}

int findIngred(const char *name) {
    for (int i = 0; i < ingredsCount; i++)
        if (strcmp(ingredients[i].name, name) == 0) 
            return i;  // ingredient is available, return index

    return -1;  // ingredient is not available
}

void addSupplier(const char *username, unsigned short port) {
    SupplierInfo* newSupplier = &suppliers[suppliersCount];
    suppliersCount++;
    memset(newSupplier->username, '\0', ID_SIZE);
    strcpy(newSupplier->username, username);
    newSupplier->port = port;
}

void addIngred(ReqIngredData* data) {
    int idx = findIngred(data->name);
    if (idx != -1) {
        ingredients[idx].amount += data->amount;
        return;
    }
    Ingredient* newIngredient = &ingredients[ingredsCount];
    ingredsCount++;
    memset(newIngredient->name, '\0', ID_SIZE);
    strcpy(newIngredient->name, data->name);
    newIngredient->amount = data->amount;
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
void alarmHandlerRest(int sig) {
    timeOut = 1;
}

void handleIngredResponse(int suppSock, ReqIngredData* ingred) {
    char buffer[BUFFER_SIZE];
    memset(buffer, '\0', BUFFER_SIZE);
    write(1, "waiting for the supplier's response...\n", strlen("waiting for the supplier's response...\n"));
    signal(SIGALRM, alarmHandlerRest);
    siginterrupt(SIGALRM, 1);
    struct termios originalTerminos = blockTerminal();
    alarm(20); // TODO change this
    int newSock = accClient(tcpSock);
    int bytesReceived = recv(newSock, buffer, BUFFER_SIZE, 0);
    if (timeOut) {
        send(suppSock, "time out", strlen("time out"), 0);
        logTerminalError("Time out!");
        timeOut = 0;
        tcsetattr(STDIN_FILENO, TCSAFLUSH, &originalTerminos);
        return;
    }
    alarm(0);
    if (bytesReceived == 0) 
        logTerminalWarning("supplier closed!\n");
    // else 
    //     write(1, &buffer[ID_SIZE], BUFFER_SIZE);
    
    // getIng(buffer, ingreq);
    if (strncmp(&buffer[ID_SIZE], "accept", strlen("accept")) == 0) {
        write(1, &buffer[ID_SIZE+6], BUFFER_SIZE - 106);
        addIngred(ingred);

    }
    else if (strncmp(&buffer[ID_SIZE], "reject", strlen("reject")) == 0)
        write(1, &buffer[ID_SIZE+6], BUFFER_SIZE - 106);

    
        
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &originalTerminos);
}

void sendReqIngred() {
    char buffer[BUFFER_SIZE];
    memset(buffer, '\0', BUFFER_SIZE);
    ReqIngredData* data = getReqIngredData();
    int newSocket = cnctServer(data->port);
    sprintf(&buffer[ID_SIZE], "request ingredient-%hu-%s-%d-%hu-",data->port, data->name, data->amount, myTcpPort);
    memcpy(buffer, identifier, ID_SIZE);
    send(newSocket, buffer, BUFFER_SIZE, 0);
    handleIngredResponse(newSocket, data);

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
    /////////////////////
    else if (strncmp(&answer.buffer[ID_SIZE], "request ingredient", strlen("request ingredient")) == 0) {

        answer.result = 0; // TODO enum
        sendReqIngred();
        // strncpy(answer.buffer, identifier, ID_SIZE); 
        // ID-request-port_yaroo-data_ha
        // sprintf(&answer.buffer[ID_SIZE], "-request ingredient-%hu-%s-%d-", data->port, data->name, data->amount);
    }
    ///////////////////


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
void handleUDP(int fd, struct sockaddr_in bcAddress, unsigned short tcpPort, char* buffer, char* username){
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
    // write(0, buffer, BUFFER_SIZE);
}

void initRest() {
    menu = readJson("recipes.json");
    memset(identifier, '\0', sizeof(identifier));
    sprintf(identifier, "%d", getpid());
}

int main(int argc, char const *argv[]) {
    CLIResult ans;
    initRest();
    int  udpSock, maxSock;
    char buffer[BUFFER_SIZE] = {0};
    struct sockaddr_in bcAddress, tcpAddress;
    fd_set workingSet, masterSet;

    tcpSock = makeTCP(&tcpAddress);
    udpSock = makeBroadcast(&bcAddress, 1234); // TODO -> argv[1]
    myTcpPort = htons(tcpAddress.sin_port);
    printf("myTcpPort: %hu\n", myTcpPort);
    
    char username[100];
    getUsername(username);
    while(sendUsernameCheck(udpSock, tcpSock, bcAddress, username, identifier, myTcpPort))
        getUsername(username);
    write(0, ANSI_GRN "\tWelcome!\n\n" ANSI_RST, strlen("\tWelcome!\n\n") + ANSI_LEN);
    
    sendHelloRestaurant(udpSock, bcAddress, myTcpPort, buffer, username);

    FD_ZERO(&masterSet);
    FD_SET(STDIN_FILENO, &masterSet);
    FD_SET(tcpSock, &masterSet);
    FD_SET(udpSock, &masterSet);
    maxSock = udpSock;
    while (1) {
        workingSet = masterSet;

        select(maxSock + 1, &workingSet, NULL, NULL, NULL);
        
        if (FD_ISSET(0, &workingSet)) {
            memset(buffer, '\0', BUFFER_SIZE);
            ans = handleCLI(username);
            if (ans.result == 1) { // udp broadcast
                memcpy(ans.buffer, identifier, ID_SIZE);
                sendto(udpSock, ans.buffer, BUFFER_SIZE, 0,(struct sockaddr *)&bcAddress, sizeof(bcAddress));
            }
            else if (ans.result == 2) { // tcp connect
                strtok(&ans.buffer[ID_SIZE], "-");
                unsigned short port = (unsigned short)atoi(strtok(NULL, "-"));
                int newSocket = cnctServer(port);
                sprintf(&ans.buffer[strlen(ans.buffer)], "-%hu-",myTcpPort);
                memcpy(ans.buffer, identifier, ID_SIZE);
                send(newSocket, ans.buffer, strlen(ans.buffer), 0);

            }
        }

        else if (FD_ISSET(udpSock, &workingSet)) {
            memset(buffer, 0, BUFFER_SIZE);
            recv(udpSock, buffer, BUFFER_SIZE, 0);
            handleUDP(udpSock, bcAddress, htons(tcpAddress.sin_port),buffer, username);
        }
        else if (FD_ISSET(tcpSock, &workingSet)) { // new clinet
            char msg[BUFFER_SIZE];
            sprintf(msg, "in new client\n");
            write(1, msg, strlen(msg));
            int newSocket = accClient(tcpSock);
            FD_SET(newSocket, &masterSet);
            maxSock = (newSocket > maxSock) ? newSocket : maxSock;
            // if (newSocket > maxSock) maxSock = newSocket;
            sprintf(msg, "New client connected. fd = %d\n", newSocket);
            write(1, msg, strlen(msg));
        }
        else {
            for (int i = 0; i <= maxSock; i++) { // tcp get message
                int bytes_received;
                bytes_received = recv(i, buffer, BUFFER_SIZE, 0);
                if (bytes_received == 0) {  // EOF
                    char msg[MESSAGE_SIZE];
                    sprintf(msg, "client fd = %d closed\n", i);
                    write(1, msg, strlen(msg));
                    close(i);
                    FD_CLR(i, &masterSet);
                    continue;
                }
                char msg[MESSAGE_SIZE];
                sprintf(msg, "client %d: %s\n", i, buffer);
                write(1, msg, MESSAGE_SIZE);
                memset(buffer, '\0', 1024);
            }
                
            
        }
    }
    return 0;
}