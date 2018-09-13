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
	��ʼ����ͨ���׽��֣����÷�������ַ�������߳���
	��ʼ���������ݴ����׽���
	*********************/
	Prot_Client(int mainPort,char* hostName,int serverPort,int n_threads);

	/********************
	��ȡ�ļ���Ϣ����дinfo�ֶΣ���ȡ�ļ����ݽ���data
	*********************/
	void readFile(char* fileName);

	/********************
	�����������󣬸����ļ���Ϣ
	*********************/
	void send_connection();

	/********************
	�������̷߳�������
	*********************/
	void send_data();

	/********************
	���ڽ�������
	*********************/
	void recv_command(command_frame* comm);
	
	/********************
	����Э��
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