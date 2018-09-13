#ifndef _PROTOCOL_SERVER_H_
#define _PROTOCOL_SERVER_H_
#include "udp_socket.h"
#include "myTypes.h"


class Prot_Server
{
public:
	int n_threads;
	int main_port;
	sockaddr_in clientAddr;
	sockaddr_in* clientAddr_data;

	Udp_socket main_socket;
	Udp_socket* data_socket;

	pthread_t* data_pid;
	int state;	
	FileInfo info;

	unsigned int* s_packages;
	pthread_t report_pid;
public:
	/********************
	初始化主通信套接字，设置服务器地址，设置线程数;
	初始话所有数据传输套接字
	*********************/
	Prot_Server(int mainPort,int n_threads);

	/********************
	发送链接回执
	*********************/
	void send_response();

	/********************
	发送断开连接
	*********************/
	void send_close();


	/********************
	接收一个命令
	*********************/
	void recv_command(command_frame* comm);

	/********************
	接收数据
	*********************/
	void recv_data();

	/********************
	检查是否传输完成
	*********************/
	void checkcomplete();

	void run();

	void fileWrite();
};

struct Server_command_struct
{
	Prot_Server* server_prot;
	command_frame *comm;
};

struct Server_data_strcut
{
	Prot_Server* server_prot;
	int serial;
};

void * server_sending_command_thread(void * arg);
void * recv_data_thread(void *arg);
void * check_complete_thread(void *arg);
void * stateReporter(void *arg);

#endif