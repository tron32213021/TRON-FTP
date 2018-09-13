#include "protocol_client.h"

Prot_Client::Prot_Client(int mainPort, char * hostName, int serverPort, int n_threads)
{
	main_socket.setMyAddr(mainPort);
	serverAddr = Udp_socket::getAddr(hostName, serverPort);
	main_socket.setPeerAddr(serverAddr);
	this->n_threads = n_threads;
	main_socket.init_udp_socket();
	data_socket = new Udp_socket[n_threads];
	serverAddr_data = new sockaddr_in[n_threads];
	sending_pid = new pthread_t[n_threads];
	recving_pid = new pthread_t[n_threads];

	for (int i = 0; i < n_threads; ++i)
	{
		serverAddr_data[i] = Udp_socket::getAddr(hostName, serverPort + i + 1);
		data_socket[i].setMyAddr(mainPort + i + 1);
		data_socket[i].setPeerAddr(serverAddr_data[i]);
		
		data_socket[i].init_udp_socket();
		
	}

}

void Prot_Client::readFile(char * fileName)
{
	printf("reading file information...\n");
	int filefd = open(fileName, O_RDONLY);
	strcpy(info.fileName, fileName);

	struct stat statbuf;
	stat(fileName, &statbuf);

	info.size_bytes = statbuf.st_size;
	info.n_packages = ceil(info.size_bytes / 1024.0);

	info.data = new Package[info.n_packages];
	info.print_info();

	for(int i=0;i<info.n_packages;++i)
		read(filefd, info.data[i].data, 1024);
	close(filefd);
}

void Prot_Client::send_connection()
{
	command_frame *comm = new command_frame();
	comm->comm = REQUEST_CONNECTION;
	strcpy(comm->data.file.fileName, info.fileName);
	comm->data.file.fileSize = info.size_bytes;
	comm->data.file.n_packages = info.n_packages;
	state = 0;

	Client_command_struct* t = new Client_command_struct();
	t->client_prot = this;
	t->comm = comm;
	pthread_t ntid;
	pthread_create(&ntid, 0, client_sending_command_thread, (void*)t);
	pthread_detach(ntid);

}

void Prot_Client::send_data()
{
	for (int i = 0; i < n_threads; ++i)
	{
		Client_data_struct* s_t = new Client_data_struct();
		s_t->client_prot = this;
		s_t->serial = i;
		pthread_create(&recving_pid[i], 0, recving_data_thread, (void*)s_t);
		pthread_detach(recving_pid[i]);
	}
	for (int i = 0; i < n_threads; ++i)
	{
		Client_data_struct* s_t = new Client_data_struct();
		s_t->client_prot = this;
		s_t->serial = i;
		pthread_create(&sending_pid[i], 0, sending_data_thread, (void*)s_t);
		pthread_detach(sending_pid[i]);
	}
}

void Prot_Client::recv_command(command_frame * comm) //only for connection response
{
	main_socket.recvMsg((char*)comm, sizeof(command_frame), &serverAddr);
}

void Prot_Client::run()
{
	command_frame comm_frame;
	printf("sending connection request command...\n");
	send_connection();
	do
	{
		recv_command(&comm_frame);
		if (comm_frame.comm == RESPONSE_CONNECTION)
		{
			printf("connection response received...\n");
			state = 1;
		}
	} while (comm_frame.comm != RESPONSE_CONNECTION);

	printf("sending data...\n");
	send_data();

	while (state != 2)
	{
		recv_command(&comm_frame);
		switch (comm_frame.comm)
		{
		case CONNECTION_CLOSE:
			printf("connection close...\n");
			for (int i = 0; i < n_threads; ++i)
			{
				pthread_cancel(sending_pid[i]);
				pthread_cancel(recving_pid[i]);
			}
			state = 2;
			exit(0);
			break;
		}
	}

}

void * client_sending_command_thread(void * arg)
{
	Client_command_struct* t = (Client_command_struct*)arg;

	while (t->client_prot->state == 0)
	{
		t->client_prot->main_socket.sendMsgToPeer((char*)t->comm, sizeof(command_frame));
		usleep(1);
	}
	delete t->comm;
	delete t;
	return 0;
}

void * sending_data_thread(void * arg)
{
	Client_data_struct* tmp = (Client_data_struct*)arg;
	Prot_Client* t = tmp->client_prot;
	while (t->state == 0);
	int serial = tmp->serial;
	data_frame dFrame;
	while (t->state != 2)
	{
		for (int i = serial; i < t->info.n_packages; i += t->n_threads)
		{
			if (!t->info.data[i].received)
			{
				dFrame.n_package = i;
				bcopy(t->info.data[i].data, dFrame.data, 1024);
				t->data_socket[serial].sendMsgToPeer((char*)&dFrame, sizeof(data_frame));
				printf("package sent to thread%d...n_package: %d\n", serial, dFrame.n_package);
				usleep(100);
			}
		}
		
	}
	delete tmp;
	return 0;
}

void * recving_data_thread(void * arg)
{
	Client_data_struct* tmp=(Client_data_struct*)arg;
	Prot_Client* t = tmp->client_prot;
	int serial = tmp->serial;
	command_frame comm;
	while (t->state == 0);
	while (t->state != 2)
	{
		t->data_socket[serial].recvMsgFromPeer((char*)&comm, sizeof(command_frame));
		if (comm.comm == RECEIPT_RESPONSE)
		{
			//printf("***************************serial:%d, package:%d\n", serial,comm.data.receipt.no_package);
			for(int i=0;i<10;++i)
				t->info.data[comm.data.receipt.no_package[i]].received = true;
		}
	}
	delete tmp;
	return 0;
}
