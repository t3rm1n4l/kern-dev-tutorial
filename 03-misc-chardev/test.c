#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include "hello.h"

#define RAND "hello1234567890"
#define DEV "/dev/"DEVICE_NAME

int main() {

	int fd;
	int n;
	char buf[1024];
	fd = open(DEV, O_RDONLY);
        n = read(fd, buf, 1024);
	printf("Read %d bytes, %s\n", n, buf);
	close(fd);

	fd = open(DEV, O_WRONLY);
	n = write(fd, buf, n);
	printf("Writing %s returned %d\n", ID_STR, n);
	close(fd);

	fd = open(DEV, O_WRONLY);
	n = write(fd, RAND, sizeof(RAND));
	printf("Writing %s returned %d\n", RAND, n);
	close(fd);

	return 0;
}
