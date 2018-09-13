#ifndef _SOCKGET_INIT_H
#define _SOCKGET_INIT_H
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <iostream>

int init_tcp_serverSide(struct sockaddr_in* addr);
int init_tcp_clientSide(struct sockaddr_in* addr);
int init_udp(struct sockaddr_in* addr);



#endif