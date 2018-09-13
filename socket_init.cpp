#include "socket_init.h"

int init_tcp_serverSide(struct sockaddr_in* addr)
{
	int fd = socket(AF_INET, SOCK_STREAM, 0);
	if (fd <= 0)
	{
		fprintf(stderr, "Socket error:%s\r\n", strerror(errno));
		exit(1);
	}
	if (bind(fd, (struct sockaddr*)addr, sizeof(struct sockaddr)) == -1)
	{
		fprintf(stderr, "Bind error:%s\r\n", strerror(errno));
		exit(1);
	}
	if (listen(fd, 5) == -1)
	{
		fprintf(stderr, "Listen error:%s\r\n", strerror(errno));
		exit(1);
	}
	return fd;
}

int init_tcp_clientSide(struct sockaddr_in* addr)
{
	int fd = socket(AF_INET, SOCK_STREAM, 0);
	if (connect(fd, (struct sockaddr*)addr, sizeof(struct sockaddr)) == -1)
	{
		fprintf(stderr, "Connection error:%s\r\n", strerror(errno));
		exit(1);
	}
	return fd;
}

int init_udp(struct sockaddr_in* addr)
{
	int fd = socket(AF_INET, SOCK_DGRAM, 0);
	if (bind(fd, (struct sockaddr*)addr, sizeof(struct sockaddr)) == -1)
	{
		fprintf(stderr, "Bind error:%s\r\n", strerror(errno));
		exit(1);
	}
	return fd;
}
