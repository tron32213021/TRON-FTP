#include "udp_socket.h"

Udp_socket::Udp_socket()
{
	bzero(&myAddr, sizeof(sockaddr_in));
	addr_len = sizeof(sockaddr_in);
}

void Udp_socket::setMyAddr(int port)
{
	myAddr.sin_family = AF_INET;
	myAddr.sin_port = htons(port);
	myAddr.sin_addr.s_addr = htonl(INADDR_ANY);

}

void Udp_socket::setPeerAddr(char * hostName, unsigned int port)
{
	peerAddr = getAddr(hostName, port);
}

void Udp_socket::setPeerAddr(sockaddr_in addr)
{
	peerAddr = addr;
}


void Udp_socket::init_udp_socket()
{
	sockfd = init_udp(&myAddr);
}

void Udp_socket::sendMsg(const char * msg, int len, sockaddr_in *addr)
{
	sendto(sockfd, msg, len, 0, (sockaddr*)addr, sizeof(sockaddr_in));
}

void Udp_socket::sendMsgToPeer(const char * msg, int len)
{
	sendMsg(msg, len, &peerAddr);
}

void Udp_socket::recvMsg(char * ptr, int len, sockaddr_in *addr)
{
	recvfrom(sockfd, ptr, len, 0, (sockaddr*)addr, &addr_len);
}

void Udp_socket::recvMsgFromPeer(char * ptr, unsigned int len)
{
	recvMsg(ptr, len, &peerAddr);
}

sockaddr_in Udp_socket::getAddr(char * hostName, unsigned int port)
{
	sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	hostent* p = gethostbyname(hostName);
	bcopy((char*)p->h_addr, (char*)&addr.sin_addr.s_addr, p->h_length);
	return addr;

}
