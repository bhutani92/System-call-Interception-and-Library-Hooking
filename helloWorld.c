#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

int main(void) {
	printf("Hello World!\n");
	printf("Opening a file!\n");
	struct stat st;
	if (stat("README.md", &st) == -1) {
		printf("Stat error... Exiting!\n");
		return 0;
	}
	int fd = open("README.md", O_RDWR);
	if (fd > 0) {
		printf("File opened successfully\n");
	} else {
		printf("Error occurred while opening file\n");
	}
	close(fd);
	return 0;
}
