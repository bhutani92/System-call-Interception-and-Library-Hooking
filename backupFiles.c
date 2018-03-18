#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dlfcn.h>

int open(const char *pathname, int flags, mode_t mode) {
	int (*backup_open)(const char *pathname, int flags, mode_t mode);
	backup_open = dlsym(RTLD_NEXT, "open");

}
