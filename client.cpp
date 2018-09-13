#include "protocol_client.h"
#include "myTypes.h"
void * stateReporter(void *arg)
{
	Prot_Client* t = (Prot_Client*)arg;
	while (1)
	{
		printf("state == %d\n", t->state);
		sleep(10);
	}
}

int main(int argc, char* argv[])
{
	if (argc < 3)
	{
		printf("argument number error...\n");
		exit(1);
	}
	int n_threads;
	if (argc == 4)
		n_threads = atoi(argv[3]);
	else
		n_threads = 2;
	printf("serverName: %s, fileName: %s\n", argv[1], argv[2]);
	char serverHostName[256];
	char fileName[256];
	bzero(serverHostName, 256);
	bzero(fileName, 256);
	strcpy(serverHostName, argv[1]);
	strcpy(fileName, argv[2]);

	Prot_Client client(50000,serverHostName,60000, n_threads);
	//pthread_t pid;
	//pthread_create(&pid, 0, stateReporter, (void*)&client);
	client.readFile(fileName);
	client.run();

	return 0;
}
