#ifndef __UDP_H__
#define __UDP_H__

#include <arpa/inet.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <string.h>
#include <asm/socket.h>
#include <stdlib.h>
#include <unistd.h>

int makeUDP(struct sockaddr_in* addrOut, unsigned short port);

#endif // __UDP_H__