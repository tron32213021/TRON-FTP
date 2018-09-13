#ifndef _PROTOCOL_CLIENT_H_
#define _PROTOCOL_CLIENT_H_
#include "udp_socket.h"
#include "myTypes.h"

class Prot_Client
{
public:
	sockaddr_in serverAddr;
	sockaddr_in* serverAddr_data;

	char serverName[256];
	int n_threads;

	Udp_socket main_socket;
	Udp_socket* data_socket;


	pthread_t* sending_pid;
	pthread_t* recving_pid;
	int state;
	FileInfo info;

public:
	/********************
	初始化主通信套接字，设置服务器地址，设置线程数
	初始化所有数据传输套接字
	*********************/
	Prot_Client(int mainPort,char* hostName,int serverPort,int n_threads);

	/********************
	读取文件信息，填写info字段，读取文件数据进入data
	*********************/
	void readFile(char* fileName);

	/********************
	发送连接请求，附加文件信息
	*********************/
	void send_connection();

	/********************
	启动多线程发送数据
	*********************/
	void send_data();

	/********************
	用于接收命令
	*********************/
	void recv_command(command_frame* comm);
	
	/********************
	运行协议
	*********************/
	void run();
};

struct Client_command_struct
{
	Prot_Client* client_prot;
	command_frame *comm;
};

struct Client_data_struct
{
	Prot_Client* client_prot;
	int serial;
};

void * client_sending_command_thread(void * arg);
void * sending_data_thread(void* arg);
void * recving_data_thread(void* arg);
#endif