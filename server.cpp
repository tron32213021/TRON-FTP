#include "protocol_server.h"
#include "myTypes.h"


int main(int argc, char* argv[])
{
	int n_threads;
	if (argc != 2)
		n_threads = 2;
	else
		n_threads = atoi(argv[1]);
	Prot_Server server(60000, n_threads);

	server.run();
	server.fileWrite();
	sleep(1);
	return 0;
}

