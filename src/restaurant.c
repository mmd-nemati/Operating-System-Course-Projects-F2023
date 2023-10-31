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
        char identifier[100];
RestaurantState state = CLOSED;
SupplierInfo suppliers[MAX_SUPPLIER];
int suppliersCount = 0;

OrderInfo orders[MAX_ORDER];
int ordersCount = 0;

SaleInfo sales[MAX_SALE];
int salesCount = 0;

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

void printSuppliers() {
    if (suppliersCount == 0) {
        printf("%sNo available supplier%s\n", ANSI_RED, ANSI_RST);
        return;
    }
    printf("--------------------\n");
    printf("<username>::<port>\n");
    for (int i = 0; i < suppliersCount; i++) 
        printf("%s%s%s::%s%hu%s\n", ANSI_YEL, suppliers[i].username, ANSI_RST, ANSI_GRN, suppliers[i].port, ANSI_RST);
    printf("--------------------\n");
}

void printOrders() {
    if (ordersCount == 0) {
        printf("%sNo available order%s\n", ANSI_YEL, ANSI_RST);
        return;
    }
    printf("--------------------\n");
    printf("<username>::<port> --> <food>\n");
    for (int i = 0; i < ordersCount; i++) 
        printf("%s%s%s::%s%hu%s --> %s%s%s\n", ANSI_YEL, orders[i].username, ANSI_RST, ANSI_GRN,
                     orders[i].port, ANSI_RST, ANSI_PUR, orders[i].food, ANSI_RST);
    printf("--------------------\n");
}

void printSales() {
    if (salesCount == 0) {
        printf("%sNo available sale%s\n", ANSI_YEL, ANSI_RST);
        return;
    }
    char *accStr = "accepted";
    char *denStr = "rejected";
    printf("--------------------\n");
    printf("<username>--<food> --> <result>\n");
    for (int i = 0; i < salesCount; i++)  {
         printf("%s%s%s--%s%s%s --> \n", ANSI_YEL, sales[i].username, ANSI_RST, ANSI_GRN,
                    sales[i].food, ANSI_RST);
        (sales[i].result == ACCEPTED) ? printf("%s%s%s", ANSI_GRN, "accepted", ANSI_RST) :
                                            printf("%s%s%s", ANSI_RED, "rejected", ANSI_RST);
    }
    printf("--------------------\n");
}

int sendUsernameCheck(int bcFd, int tcpFd, struct sockaddr_in addr, 
                         char *username, unsigned short tcpPort) {
    char buffer[BUFFER_SIZE];
    memset(buffer, '\0', BUFFER_SIZE);
    // strncpy(username, username, strlen(username));
    strncpy(buffer, identifier, ID_SIZE);
    sprintf(&buffer[ID_SIZE], "username check-%s-%hu", username, tcpPort);
    // write(0, buffer, 150);
    sendto(bcFd, buffer, BUFFER_SIZE, 0,(struct sockaddr *)&addr, sizeof(addr));
    signal(SIGALRM, alarm_handler);
    siginterrupt(SIGALRM, 1);
    alarm(2);
    int clientFd = accClient(tcpFd);
    alarm(0);
    if (clientFd >= 0) {
        logTerminalError("Username already used. Try again.");
        return 1;
    }
    return 0;
}

// int handleUsernameCheck()

int makeBroadcast(struct sockaddr_in* addrOut, int port){
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

int makeTCP(struct sockaddr_in* addrOut){
    int tcpFd;
    tcpFd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in addr;
    int reuseport = 1;
    setsockopt(tcpFd, SOL_SOCKET, SO_REUSEPORT, &reuseport, sizeof(reuseport));

    addr.sin_family = AF_INET;
    addr.sin_port = htons(1236);
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK); 
    *addrOut = addr;
    bind(tcpFd, (struct sockaddr *)&addr, sizeof(addr));
    
    listen(tcpFd, 4);
    return tcpFd;
}

int connectServer222(int fd, struct sockaddr_in server, int port) {
    
    server.sin_family = AF_INET; 
    server.sin_port = htons(port); 
    server.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

    if (connect(fd, (struct sockaddr *)&server, sizeof(server)) < 0) { // checking for errors
        printf("Error in connecting to server\n");
    }

    return fd;
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
        state = CLOSED;
    }
    else if (strncmp(&answer.buffer[ID_SIZE], "show suppliers", strlen("show suppliers")) == 0) {
        printSuppliers();
        // TODO log file
    }
    else if (strncmp(&answer.buffer[ID_SIZE], "show requests list", strlen("show requests list")) == 0) {
        printOrders();
        // TODO log file
    }
    else if (strncmp(&answer.buffer[ID_SIZE], "show sales history", strlen("show sales history")) == 0) {
        printSales();
        // TODO log file
    }
    return answer;
    // TODO log file
}
void handleIncomingBC(char* buffer){
    if (strncmp(buffer, identifier, strlen(identifier)) == 0) // check self broadcasting
        return; 

    if (state == OPEN && strncmp(&buffer[ID_SIZE], "hello supplier", strlen("hello supplier")) == 0) {
            strtok(&buffer[ID_SIZE], "-");
            char* username = strtok(NULL, "-"); // username
            char* port = strtok(NULL, "-");
            if (!isSupplierUnique(username))
                return;
            
            addSupplier(username, atoi(port));
            return;
    }
    write(0, buffer, BUFFER_SIZE);
}

int main(int argc, char const *argv[]) {
    CLIResult ans;

    int tcpSock, bcSock, broadcast = 1, opt = 1;
    char buffer[1024] = {0};
    struct sockaddr_in bcAddress, tcpAddress;
    fd_set readfds;
    memset(identifier, '\0', sizeof(identifier));
    sprintf(identifier, "%d", getpid());
    // char username[100];
    // memset(username, '\0', ID_SIZE);
    // // read(0, username, ID_SIZE);
    // // strncpy(username, username, strlen(username) - 1);
    // memcpy(username, "hamid", strlen("rest1"));
    char username[100];
    write(0, "Enter your username: ", strlen("Enter your username: "));
    memset(username, '\0', ID_SIZE);
    // read(0, username, ID_SIZE);
    read(0, username, ID_SIZE);
            // write(0, "anomoly", 7);

    for (int i = 0; i < ID_SIZE; i++) 
        if (username[i] == '\n') {
            username[i] = '\0';
            break;
        }
    tcpSock = makeTCP(&tcpAddress);
    bcSock = makeBroadcast(&bcAddress, 1234);
    // setsockopt(sock, SOL_SOCKET, SO_BROADCAST, &broadcast, sizeof(broadcast));
    // setsockopt(tcpSock, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt));
    bind(bcSock, (struct sockaddr *)&bcAddress, sizeof(bcAddress));
    // bind(tcpSock, (struct sockaddr *)&tcpSock, sizeof(tcpSock));
    while(sendUsernameCheck(bcSock, tcpSock, bcAddress, username, 1236)) {
        memset(username, '\0', ID_SIZE);
        read(0, username, ID_SIZE);

        for (int i = 0; i < ID_SIZE; i++) 
            if (username[i] == '\n') {
                username[i] = '\0';
                break;
            }
    }
    write(0, "welcome!\n", strlen("welcome!\n"));
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
            handleIncomingBC(buffer);
        }
    }
    // write(0, "Enter username: ", sizeof("Enter username: "));
    // memcpy(buffer+strlen(identifier), "hello", strlen("hello"));
    // read(0, username, 100);
    // memcpy(buffer+strlen(identifier)+strlen("hello"), username, strlen(username));
    
    // sendto(tcpSock, buffer, strlen(buffer), 0,(struct sockaddr *)&bcAddress, sizeof(bcAddress));

    return 0;
}