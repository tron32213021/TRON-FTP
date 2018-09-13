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
	��ʼ����ͨ���׽��֣����÷�������ַ�������߳���;
	��ʼ���������ݴ����׽���
	*********************/
	Prot_Server(int mainPort,int n_threads);

	/********************
	�������ӻ�ִ
	*********************/
	void send_response();

	/********************
	���ͶϿ�����
	*********************/
	void send_close();


	/********************
	����һ������
	*********************/
	void recv_command(command_frame* comm);

	/********************
	��������
	*********************/
	void recv_data();

	/********************
	����Ƿ������
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