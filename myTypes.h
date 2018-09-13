#ifndef _FILE_INFO_H_
#define _FILE_INFO_H_
#include <fcntl.h>
#include <sys/stat.h>
#include <math.h>
#include <pthread.h>
#include "frame.h"
#include <time.h>
#include <string.h>
#define DEBUG
class Package
{
public:
	bool received;
	unsigned int data[256];
public:
	Package();
};

class FileInfo
{
public:
	char fileName[256];
	Package* data;
	unsigned int size_bytes;
	unsigned int n_packages;
public:
	void print_info();
};

#endif