#ifndef __USER_H__
#define __USER_H__

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

#include "types.h"
#include "alarm.h"
#include "logger.h"
#include "tcp.h"

void getUsername(char *username);

int sendUsernameCheck(int bcFd, int tcpFd, struct sockaddr_in addr, 
                         char *username, char *identifier, unsigned short tcpPort);

void handleUsernameCheck(char *buffer, char *username);

#endif // __USER_H__