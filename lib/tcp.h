#ifndef __TCP_H__
#define __TCP_H__

#include <arpa/inet.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <string.h>
#include <asm/socket.h>
#include <stdlib.h>
#include <unistd.h>

int makeTCP(struct sockaddr_in* addrOut);
int accClient(int fd);
int cnctServer(int port);


#endif // __TCP_H__