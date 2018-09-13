#ifndef _UDP_SOCKET_H_
#define _UDP_SOCKET_H_
#include "socket_init.h"
class Udp_socket
{
public:
	sockaddr_in myAddr;
	sockaddr_in peerAddr;
	socklen_t addr_len;
	int sockfd;
public:
	Udp_socket();
	void setMyAddr(int port);
	void setPeerAddr(char* hostName, unsigned int port);
	void setPeerAddr(sockaddr_in addr);
	void init_udp_socket();
	void sendMsg(const char* msg, int len, sockaddr_in *addr);
	void sendMsgToPeer(const char* msg, int len);
	void recvMsg(char* ptr, int len,sockaddr_in *addr);
	void recvMsgFromPeer(char* ptr, unsigned int len);
	static sockaddr_in getAddr(char* hostName, unsigned int port);
};

#endif