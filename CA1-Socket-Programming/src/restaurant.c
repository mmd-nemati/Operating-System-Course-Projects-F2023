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
char username[100];
Menu* menu;
int tcpSock;
unsigned short myTcpPort;
int timeOut = 0;

RestaurantState state = CLOSED;
SupplierInfo suppliers[MAX_SUPPLIER];
int suppliersCount = 0;

OrderInfo orders[MAX_ORDER];
int ordersCount = 0;

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

int findOrder(const char *username) {
    for (int i = 0; i < ordersCount; i++)
        if (strcmp(orders[i].username, username) == 0) 
            return i;  // order is available, return index

    return -1;  // order is not available
}

int findOrderByPort(unsigned short port) {
    for (int i = 0; i < ordersCount; i++)
        if (orders[i].port ==  port)
            if (orders[i].result == PENDING) 
                return i;  // order is available, return index

    return -1;  // order is not available
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

void addOrder(OrderInfo* newOrder) {
    if (ordersCount == MAX_ORDER) {
        logTerminalError("Cannot store more orders");
        return;
    }
    orders[ordersCount] = *newOrder;
    ordersCount++;
}

void expireOrder(const char* username) {
    int idx = findOrder(username);
    if (idx == -1) {
        logTerminalError("Cannot find order");
        return;
    }
    orders[idx].result = DENIED;
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
    struct termios originalTerminos = lockTerminal();
    alarm(90); 
    int newSock = accClient(tcpSock);
    int bytesReceived = recv(newSock, buffer, BUFFER_SIZE, 0);
    if (timeOut) {
        send(suppSock, "time out", strlen("time out"), 0);
        logTerminalError("Time out!");
        timeOut = 0;
        tcsetattr(STDIN_FILENO, TCSAFLUSH, &originalTerminos);
        logFile("'request ingredient' request time out!", username);
        return;
    }
    alarm(0);
    if (bytesReceived == 0) 
        logTerminalWarning("supplier closed!\n");

    if (strncmp(&buffer[ID_SIZE], "accept", strlen("accept")) == 0) {
        write(1, &buffer[ID_SIZE+6], BUFFER_SIZE - 106);
        addIngred(ingred);

    }
    else if (strncmp(&buffer[ID_SIZE], "reject", strlen("reject")) == 0)
        write(1, &buffer[ID_SIZE+6], BUFFER_SIZE - 106);

    tcsetattr(STDIN_FILENO, TCSAFLUSH, &originalTerminos);
    logFile("'request ingredient' response received", username);
}

void sendReqIngred() {
    char buffer[BUFFER_SIZE];
    memset(buffer, '\0', BUFFER_SIZE);
    ReqIngredData* data = getReqIngredData();
    int newSocket = cnctServer(data->port);
    sprintf(&buffer[ID_SIZE], "request ingredient-%hu-%s-%d-%hu-",data->port, data->name, data->amount, myTcpPort);
    memcpy(buffer, identifier, ID_SIZE);
    send(newSocket, buffer, BUFFER_SIZE, 0);
    logFile("'request ingredient' request sent", username);
    handleIngredResponse(newSocket, data);
}

ResponseOrderData* getResponseOrderData() {
    ResponseOrderData* response = (ResponseOrderData*)malloc(sizeof(ResponseOrderData));
    char buffer[MESSAGE_SIZE];
    char inp[BUFFER_SIZE];
    memset(buffer, '\0', MESSAGE_SIZE);
    memset(inp, '\0', BUFFER_SIZE);
    
    sprintf(inp, "--> %sport%s of request: ", ANSI_GRN, ANSI_RST);
    write(STDOUT_FILENO, inp, BUFFER_SIZE);
    read(0, buffer, BUFFER_SIZE);
    response->port = (unsigned short)atoi(strtok(buffer, "\n"));

    memset(buffer, '\0', MESSAGE_SIZE);
    memset(inp, '\0', BUFFER_SIZE);
    sprintf(inp, "--> your answer (%saccept%s/%sreject%s): ", ANSI_GRN, ANSI_RST, ANSI_RED, ANSI_RST);
    write(STDOUT_FILENO, inp, BUFFER_SIZE);
    read(0, buffer, BUFFER_SIZE);
    strcpy(response->result, strtok(buffer, "\n"));
}

int findFood(const char* name) {
    for (int i = 0; i < menu->foodsNum; i++)
        if (strcmp(menu->foods[i]->name, name) == 0)
            return i; // found food

    return -1; // not found
}

int checkEnoughIngred(const char* food) {
    int ingredIdx, foodIdx = findFood(food);
    if (foodIdx == -1) {
        logTerminalError("Food not found.");
        return 0;
    }
    for (int i = 0; i < menu->foods[foodIdx]->ingredsNum; i++) {
        ingredIdx = findIngred(menu->foods[foodIdx]->ingredients[i].name);
        // printf("ingred: %s menu: %d, now: %d\n", menu->foods[foodIdx]->ingredients[i].name,
        //     menu->foods[foodIdx]->ingredients[i].amount, ingredients[ingredIdx].amount);

        if (menu->foods[foodIdx]->ingredients[i].amount > ingredients[ingredIdx].amount) {
            logTerminalError("Not enough ingredients.");
            return 0;
        }
    }

    return 1;
}
void makeFood(const char* food) {
    int ingredIdx, foodIdx = findFood(food);
    for (int i = 0; i < menu->foods[foodIdx]->ingredsNum; i++) {
        ingredIdx = findIngred(menu->foods[foodIdx]->ingredients[i].name);
        ingredients[ingredIdx].amount -= menu->foods[foodIdx]->ingredients[i].amount;
    }
}


CLIResult handleCLI(){
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
        if (getNumOfOrdersResult(orders, ordersCount, PENDING) != 0) {
            logTerminalError("Restaurant has pending orders");
            answer.result = 0;
            return answer;
        }
        sprintf(&answer.buffer[ID_SIZE], "break-%s-", username);
        logTerminalInfo("Restaurant is now closed");
        state = CLOSED;
    }
    /////////////////////
    else if (strncmp(&answer.buffer[ID_SIZE], "request ingredient", strlen("request ingredient")) == 0) {
        answer.result = 0; 
        sendReqIngred();
        logFile("'request ingredient' command completed", username);
    }

    else if (strncmp(&answer.buffer[ID_SIZE], "answer request", strlen("answer request")) == 0) {

        char buffer[MESSAGE_SIZE];
        char inp[BUFFER_SIZE];
        memset(buffer, '\0', MESSAGE_SIZE);
        memset(inp, '\0', BUFFER_SIZE);
        
        ResponseOrderData* data = getResponseOrderData();
        // unsigned short port = (unsigned short)atoi(strtok(buffer, "\n"));
        int idx = findOrderByPort(data->port);
        if (idx == -1) {
            logTerminalError("No request found for this port");
            logFile("'answer request' command failed.[port]", username);
            return answer;
        }

        int custSock = cnctServer(data->port);
        logFile("Connected to the customer.", username);
        memcpy(buffer, identifier, ID_SIZE);
        if (strncmp(data->result, "accept", strlen("accept")) == 0 && checkEnoughIngred(orders[idx].food)) {
            write(1, "Accepted request!\n\n", strlen("Accepted request!\n\n"));
            sprintf(&buffer[ID_SIZE], "accept%s Restaurant accepted!\n", username);
            orders[idx].result = ACCEPTED;
            makeFood(orders[idx].food);
        } 
        else {
            write(1, "Rejected request!\n\n", strlen("Rejected request!\n\n"));
            sprintf(&buffer[ID_SIZE], "reject%s Restaurant denied!\n", username);
            orders[idx].result = DENIED;
        }
        send(custSock, buffer, MESSAGE_SIZE, 0);
        logFile("'answer request' command completed.", username);
        answer.result = 0;
    }

    else if (strncmp(&answer.buffer[ID_SIZE], "show suppliers", strlen("show suppliers")) == 0) {
        answer.result = 0;
        printSuppliers(suppliers, suppliersCount);
        logFile("'show suppliers' command completed.", username);
    }
    else if (strncmp(&answer.buffer[ID_SIZE], "show requests list", strlen("show requests list")) == 0) {
        printOrders(orders, ordersCount);
        logFile("'show requests list' command completed.", username);
    }
    else if (strncmp(&answer.buffer[ID_SIZE], "show sales history", strlen("show sales history")) == 0) {
        printSales(orders, ordersCount);
        logFile("'show sales history' command completed.", username);
    }
    else if (strncmp(&answer.buffer[ID_SIZE], "show ingredients", strlen("show ingredients")) == 0) {
        printIngredients(ingredients, ingredsCount);
        logFile("'show ingredients' command completed.", username);
    }
    else if (strncmp(&answer.buffer[ID_SIZE], "show recipes", strlen("show recipes")) == 0) {
        printRecipes(menu);
        logFile("'show recipes' command completed.", username);
    }
    return answer;
}
void handleUDP(int fd, struct sockaddr_in bcAddress, unsigned short tcpPort, char* buffer){
    if (strncmp(buffer, identifier, strlen(identifier)) == 0) // check self broadcasting
        return; 

    if (strncmp(&buffer[ID_SIZE], "username check", strlen("username check")) == 0) {
        handleUsernameCheck(buffer, username);
        logFile("'username check' command completed", username);
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
        sendHelloRestaurant(fd, bcAddress, tcpPort, buffer, username);
        logFile("Sent hello restaurant.", username);
        return;
    }
}

void handleTCP(char *buffer) {
    if (strncmp(&buffer[ID_SIZE], "order food", strlen("order food")) == 0) {
        // "order food-%hu-%s-%s-%hu-",data->port, data->username, data->food, myTcpPort);
        OrderInfo* newOrder = (OrderInfo*)malloc(sizeof(OrderInfo));
        memset(newOrder->username, '\0', ID_SIZE);
        memset(newOrder->food, '\0', MAX_FOOD_NAME);

        strtok(&buffer[ID_SIZE], "-");
        strtok(NULL, "-");
        strcpy(newOrder->username, strtok(NULL, "-"));
        strcpy(newOrder->food, strtok(NULL, "-"));
        newOrder->port = (unsigned short)atoi(strtok(NULL, "-"));
        newOrder->result = PENDING;
        addOrder(newOrder);

        logTerminalInfo("\tNew order received.\n");
        logFile("'order food' request received", username);
    }
    else if (strncmp(&buffer[ID_SIZE], "time out order", strlen("time out order")) == 0) {
        strtok(&buffer[ID_SIZE], "-");
        char usernameToken[100];
        // strcpy(usernameToken, strtok(NULL, "-"));
        expireOrder(strtok(NULL, "-"));
        logTerminalWarning("Food request expired.\n");
        logFile("'order food' request expired", username);
    }
}

void initRest() {
    menu = readJson("recipes.json");
    memset(identifier, '\0', sizeof(identifier));
    sprintf(identifier, "%d", getpid());
    memset(username, '\0', sizeof(identifier));
    logFile("initialized", username);
}

int main(int argc, char const *argv[]) {
    CLIResult ans;
    initRest();
    int udpSock, maxSock;
    char buffer[BUFFER_SIZE] = {0};
    struct sockaddr_in bcAddress, tcpAddress;
    fd_set workingSet, masterSet;

    tcpSock = makeTCP(&tcpAddress);
    udpSock = makeUDP(&bcAddress, (unsigned short)atoi(argv[1])); // TODO -> atoi(argv[1])
    myTcpPort = htons(tcpAddress.sin_port);
    
    getUsername(username);
    while(sendUsernameCheck(udpSock, tcpSock, bcAddress, username, identifier, myTcpPort))
        getUsername(username);
    write(0, ANSI_GRN "\tWelcome!\n\n" ANSI_RST, strlen("\tWelcome!\n\n") + ANSI_LEN);
    logFile("Logged in.", username);

    sendHelloRestaurant(udpSock, bcAddress, myTcpPort, buffer, username);
    logFile("Sent hello restaurant.", username);

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
            handleUDP(udpSock, bcAddress, htons(tcpAddress.sin_port),buffer);
        }
        else if (FD_ISSET(tcpSock, &workingSet)) { // new clinet
            int newSocket = accClient(tcpSock);
            FD_SET(newSocket, &masterSet);
            maxSock = (newSocket > maxSock) ? newSocket : maxSock;
            logFile("Client connected.", username);
        }
        else {
            for (int i = 3; i <= maxSock; i++) { // tcp get message
                if (FD_ISSET(i, &workingSet)) {
                    int bytesReceived;
                    memset(buffer, '\0', BUFFER_SIZE);
                    bytesReceived = recv(i, buffer, BUFFER_SIZE, 0);
                    if (bytesReceived == 0) {  // EOF
                        //TODO LOG
                        logFile("Client disconnected.", username);
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