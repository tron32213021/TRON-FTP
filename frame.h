#ifndef _FRAME_H_
#define _FRAME_H_
struct command_frame
{
	unsigned int comm;
	union {
		struct {
			char fileName[256];
			unsigned int fileSize;
			unsigned int n_packages;
		}file;
		struct {
			unsigned int no_package[10];
		}receipt;
	}data;
};

struct data_frame
{
	unsigned int n_package;
	unsigned int data[256];
};

//define command
#define REQUEST_CONNECTION		0x80000000
#define RESPONSE_CONNECTION		0x40000000
#define RECEIPT_RESPONSE		0x20000000
#define CONNECTION_CLOSE		0x00000001

#endif
