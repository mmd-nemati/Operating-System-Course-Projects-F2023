#include "../lib/user.h"

void getUsername(char *username) {
    write(0, "Enter your username: ", strlen("Enter your username: "));
    memset(username, '\0', ID_SIZE);
    read(0, username, ID_SIZE);
    for (int i = 0; i < ID_SIZE; i++) 
        if (username[i] == '\n') {
            username[i] = '\0';
            break;
        }
}


int sendUsernameCheck(int bcFd, int tcpFd, struct sockaddr_in addr, 
                         char *username, char *identifier, unsigned short tcpPort) {
    char buffer[BUFFER_SIZE];
    memset(buffer, '\0', BUFFER_SIZE);
    // strncpy(username, username, strlen(username));
    strncpy(buffer, identifier, ID_SIZE);
    sprintf(&buffer[ID_SIZE], "username check-%s-%hu", username, tcpPort);
    // write(0, &buffer[ID_SIZE], 150);
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

void handleUsernameCheck(char *buffer, char *username) {
    strtok(&buffer[ID_SIZE], "-");
    char* usernameToken = strtok(NULL, "-");
    unsigned short port = atoi(strtok(NULL, "-"));  
    // printf("usernameToken: %s %ld --- port %hu --- username: %s %ld\n", usernameToken,strlen(usernameToken), port, username, strlen(username));
    if (strcmp(username, usernameToken) == 0)
        cnctServer(port);
}