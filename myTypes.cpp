#include "myTypes.h"
#include <stdio.h>

void FileInfo::print_info()
{

	printf("-----------file information-------------\n");
	printf("fileName: %s\nsize: %d\nn_packages: %d\n", fileName, size_bytes, n_packages);
	printf("----------------------------------------\n");
}

Package::Package()
{
	received = false;
	bzero(data, 256);
}