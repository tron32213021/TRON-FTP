#include "protocol_server.h"

Prot_Server::Prot_Server(int mainPort, int n_threads)
{
	main_socket.setMyAddr(mainPort);
	main_socket.init_udp_socket();

	this->main_port = mainPort;
	this->n_threads = n_threads;
	clientAddr_data = new sockaddr_in[n_threads];
	data_socket = new Udp_socket[n_threads];
	data_pid = new pthread_t[n_threads];
	s_packages = new unsigned int[n_threads];
	for (int i = 0; i < n_threads; ++i)
	{
		data_socket[i].setMyAddr(main_port + i + 1);
		data_socket[i].init_udp_socket();
		s_packages[i] = 0;
	}
	state = 0;

	
}

void Prot_Server::send_response()
{
	command_frame *comm = new command_frame();
	comm->comm = RESPONSE_CONNECTION;
	Server_command_struct* t = new Server_command_struct();
	t->server_prot = this;
	t->comm = comm;

	pthread_t ntid;
	pthread_create(&ntid, 0, server_sending_command_thread, (void*)t);
	pthread_detach(ntid);
}

void Prot_Server::send_close()
{
	command_frame *comm = new command_frame();
	comm->comm = CONNECTION_CLOSE;
	Server_command_struct* t = new Server_command_struct();
	t->server_prot = this;
	t->comm = comm;

	pthread_t ntid;
	pthread_create(&ntid, 0, server_sending_command_thread, (void*)t);
	pthread_detach(ntid);
}


void Prot_Server::recv_command(command_frame* comm)
{
	main_socket.recvMsgFromPeer((char*)comm, sizeof(command_frame));
}

void Prot_Server::recv_data()
{
	for (int i = 0; i < n_threads; ++i)
	{
		Server_data_strcut* t = new Server_data_strcut();
		t->server_prot = this;
		t->serial = i;
		pthread_create(&data_pid[i], 0, recv_data_thread, (void*)t);
	}
}

void Prot_Server::checkcomplete()
{
	while (state == 1);
	pthread_t pid;
	pthread_create(&pid, 0, check_complete_thread, (void*)this);
	pthread_detach(pid);

}

void Prot_Server::run()
{
	command_frame comm;
	recv_command(&comm);
	if (comm.comm == REQUEST_CONNECTION)
	{
		printf("connection request received...\n");
		strcpy(info.fileName, comm.data.file.fileName);
		info.size_bytes = comm.data.file.fileSize;
		info.n_packages = comm.data.file.n_packages;
		info.data = new Package[info.n_packages];
		info.print_info();
		state = 1;
	}
	printf("sending connection response...\n");
	send_response();
	printf("starting receive threads...\n");
	unsigned long startTime, endTime;
	startTime = time(0);
	printf("start timestamp: %u\n", startTime);
	recv_data();
	printf("starting check_complete threads...\n");
	checkcomplete();
	pthread_create(&report_pid, 0, stateReporter, (void*)this);
	while (state != 3); 
	pthread_cancel(report_pid);
	endTime = time(0);
	printf("end timestamp: %u\n", endTime);
	printf("Running Time: %d\n", endTime - startTime);
	info.print_info();
	long vv = info.size_bytes / (1024);
	double v = 8.0 * vv / (1024*(endTime - startTime));
	char danwei[10] = "Mb/s";
	
	printf("throughput: %f%s\n", v, danwei);

}

void Prot_Server::fileWrite()
{
	printf("writing into file...\n");
	char newFileName[256] = ".recv";
	strcat(info.fileName,newFileName);
	int file_fd = open(info.fileName, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
	if (file_fd < 0)
		printf("create file failed...\n");
	else
	{
		unsigned int byteLeft = info.size_bytes;
		for (int i = 0; i < info.n_packages; ++i)
		{
			write(file_fd, info.data[i].data, (1024 < byteLeft ? 1024 : byteLeft));
			byteLeft -= 1024;
		}
		
		close(file_fd);
		printf("writing complete, %d bytes written.\nnew fileName: %s\n", info.size_bytes, info.fileName);
	}
}

void * server_sending_command_thread(void * arg)
{
	Server_command_struct* tmp = (Server_command_struct*)arg;
	Prot_Server* t = tmp->server_prot;
	command_frame* comm = tmp->comm;
	switch (comm->comm)
	{
	case RESPONSE_CONNECTION:
		while (t->state == 1)
		{
			t->main_socket.sendMsgToPeer((char*)comm, sizeof(command_frame));
			usleep(1);
		}
		break;
	case CONNECTION_CLOSE:
		for (int i = 0; i < t->n_threads; ++i)
			pthread_cancel(t->data_pid[i]);
		for (int i = 0; i < 10; ++i)
			t->main_socket.sendMsgToPeer((char*)comm, sizeof(command_frame));
		break;
	}

	return 0;
}

void * recv_data_thread(void * arg)
{
	Server_data_strcut* tmp = (Server_data_strcut*)arg;
	Prot_Server* t = tmp->server_prot;
	int serial = tmp->serial;
	data_frame dFrame;
	command_frame comm;
	comm.comm = RECEIPT_RESPONSE;
	int s = 0;
	while (t->state != 3)
	{
		t->data_socket[serial].recvMsgFromPeer((char*)&dFrame, sizeof(data_frame));
		if (t->state == 1)
			t->state = 2;
		if (!t->info.data[dFrame.n_package].received)
		{
			//printf("data received...n_package: %d\n", dFrame.n_package,t->info.n_packages);
			bcopy(dFrame.data, t->info.data[dFrame.n_package].data, 1024);
			t->info.data[dFrame.n_package].received = true;
			(t->s_packages[serial])++;
			
		}
		comm.data.receipt.no_package[s++] = dFrame.n_package;
		if (s == 10)
		{
			for (int i = 0; i < 10; ++i)
				t->data_socket[serial].sendMsgToPeer((char*)&comm, sizeof(command_frame));
			s = 0;
		}
	}
	return 0;
}

void * check_complete_thread(void * arg)
{
	Prot_Server* t = (Prot_Server*)arg;
	bool complete;
	while (1)
	{
		complete = true;
		for (int i = 0; i < t->info.n_packages; ++i)
			if (t->info.data[i].received == false)
			{
				complete = false;
				break;
			}
		if (complete)
		{
			t->state = 3;
			t->send_close();
			break;
		}
	}
	return 0;
}

void * stateReporter(void *arg)
{
	Prot_Server* t = (Prot_Server*)arg;
	unsigned int total_packages;
	double percents;
	while (t->state != 2);
	while (1)
	{
		total_packages = 0;
		for (int i = 0; i < t->n_threads; ++i)
			total_packages += t->s_packages[i];
		percents = 1.0*total_packages / t->info.n_packages;
		for (int i = 0; i < percents * 50 - 1; ++i)
			printf("=");
		printf(">");
		for (int i = 0; i < 50 - percents * 50; ++i)
			printf("-");
		printf(" | %f%\n", percents * 100);
		sleep(1);
	}
}