#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dlfcn.h>
#include <time.h>
#include <string.h>
#include <dirent.h>
#include <stdbool.h>

#define MAX_SIZE 1024

int (*backup_open)(const char *pathname, int flags, ...);
int (*backup_openat)(int dirfd, const char *pathname, int flags, ...);
int (*backup_creat)(const char *pathname, mode_t mode);
int (*backup_link)(const char *oldpath, const char *newpath);
int (*backup_linkat)(int olddirfd, const char *oldpath, int newdirfd, const char *newpath, int flags);
int (*backup_unlink)(const char *pathname);
int (*backup_unlinkat)(int dirfd, const char *pathname, int flags);
int (*backup_fflush)(FILE* stream);

int is_regular_file(const char *path) {
	struct stat st;
	stat(path, &st);
	return S_ISREG(st.st_mode);
}

bool check_file_exists(const char *pathname) {
	struct stat st;
	if (stat(pathname, &st) == -1) {
		return false;
	}
	return true;
}

int unlinkat(int dirfd, const char *pathname, int flags) {
	backup_unlinkat = dlsym(RTLD_NEXT, "unlinkat");

	if (!backup_open) {
		perror("Open not initialized");
		backup_open = dlsym(RTLD_NEXT, "open");
	}

	if (is_regular_file(pathname)) {
		char *backup_loc = (char *)malloc(MAX_SIZE * sizeof(char));
		if (backup_loc == NULL) {
			return -1;
		}

		char *home_dir = getenv("HOME");
		snprintf(backup_loc, MAX_SIZE, "%s/.backup", home_dir);
		DIR *entry = opendir(backup_loc);
		if (entry == NULL) {
			int ret = mkdir(backup_loc, S_IRWXU);
			if (ret == -1) {
				perror("Unable to create backup folder");
				free(backup_loc);
				return -1;
			}
		}

		int pos = -1;
		int num = 0;
		for (int i = strlen(pathname) - 1; i >= 0; i--) {
			if (pathname[i] == '/') {
				pos = i + 1;
				break;
			}
			num++;
		}

		char *fname = (char *)malloc((num + 1) * sizeof(char));
		if (pos == -1) {
			strncpy(fname, pathname, num);
		} else {
			strncpy(fname, pathname + pos, num);
		}
		fname[num] = '\0';

		unsigned long cur_timestamp = (unsigned long)time(NULL);
		snprintf(backup_loc + strlen(backup_loc), MAX_SIZE, "/%s_%lu", fname, cur_timestamp);
		free(fname);

		int fd_old = backup_open(pathname, O_RDONLY, S_IRWXU);
		if (fd_old == -1) {
			free(backup_loc);
			return -1;
		}

		/*char err_buf[MAX_SIZE] = {0};
		snprintf(err_buf, MAX_SIZE, "Opened file %s for reading successfully", pathname);
		perror(err_buf);*/

		int fd_new = backup_open(backup_loc, O_RDWR | O_CREAT, S_IRWXU);
		if (fd_new == -1) {
			close(fd_old);
			free(backup_loc);
			return -1;
		}

		/*snprintf(err_buf, MAX_SIZE, "Opened file %s for writing successfully", backup_loc);
		perror(err_buf);*/

		int nread = -1;
		char buf[MAX_SIZE] = {0};
		while ((nread = read(fd_old, buf, MAX_SIZE)) > 0) {
			int nwrite = write(fd_new, buf, nread);
			if (nwrite == -1) {
				nread = -1;
				break;
			}
		}

		free(backup_loc);
		close(fd_old);
		close(fd_new);

		if (nread == -1) {
			return -1;
		}
	}

	return backup_unlinkat(dirfd, pathname, flags);
}

int unlink(const char *pathname) {
	backup_unlink = dlsym(RTLD_NEXT, "unlink");

	if (!backup_open) {
		perror("Open not initialized");
		backup_open = dlsym(RTLD_NEXT, "open");
	}

	if (is_regular_file(pathname)) {
		char *backup_loc = (char *)malloc(MAX_SIZE * sizeof(char));
		if (backup_loc == NULL) {
			return -1;
		}

		char *home_dir = getenv("HOME");
		snprintf(backup_loc, MAX_SIZE, "%s/.backup", home_dir);
		DIR *entry = opendir(backup_loc);
		if (entry == NULL) {
			int ret = mkdir(backup_loc, S_IRWXU);
			if (ret == -1) {
				perror("Unable to create backup folder");
				free(backup_loc);
				return -1;
			}
		}

		int pos = -1;
		int num = 0;
		for (int i = strlen(pathname) - 1; i >= 0; i--) {
			if (pathname[i] == '/') {
				pos = i + 1;
				break;
			}
			num++;
		}

		char *fname = (char *)malloc((num + 1) * sizeof(char));
		if (pos == -1) {
			strncpy(fname, pathname, num);
		} else {
			strncpy(fname, pathname + pos, num);
		}
		fname[num] = '\0';

		unsigned long cur_timestamp = (unsigned long)time(NULL);
		snprintf(backup_loc + strlen(backup_loc), MAX_SIZE, "/%s_%lu", fname, cur_timestamp);
		free(fname);

		int fd_old = backup_open(pathname, O_RDONLY, S_IRWXU);
		if (fd_old == -1) {
			free(backup_loc);
			return -1;
		}

		/*char err_buf[MAX_SIZE] = {0};
		snprintf(err_buf, MAX_SIZE, "Opened file %s for reading successfully", pathname);
		perror(err_buf);*/

		int fd_new = backup_open(backup_loc, O_RDWR | O_CREAT, S_IRWXU);
		if (fd_new == -1) {
			close(fd_old);
			free(backup_loc);
			return -1;
		}

		/*snprintf(err_buf, MAX_SIZE, "Opened file %s for writing successfully", backup_loc);
		perror(err_buf);*/

		int nread = -1;
		char buf[MAX_SIZE] = {0};
		while ((nread = read(fd_old, buf, MAX_SIZE)) > 0) {
			int nwrite = write(fd_new, buf, nread);
			if (nwrite == -1) {
				nread = -1;
				break;
			}
		}

		free(backup_loc);
		close(fd_old);
		close(fd_new);

		if (nread == -1) {
			return -1;
		}
	}

	return backup_unlink(pathname);
}

int linkat(int olddirfd, const char *oldpath, int newdirfd, const char *newpath, int flags) {
	backup_linkat = dlsym(RTLD_NEXT, "linkat");

	if (!backup_open) {
		perror("Open not initialized");
		backup_open = dlsym(RTLD_NEXT, "open");
	}

	if (is_regular_file(oldpath)) {
		char *backup_loc = (char *)malloc(MAX_SIZE * sizeof(char));
		if (backup_loc == NULL) {
			return -1;
		}

		char *home_dir = getenv("HOME");
		snprintf(backup_loc, MAX_SIZE, "%s/.backup", home_dir);
		DIR *entry = opendir(backup_loc);
		if (entry == NULL) {
			int ret = mkdir(backup_loc, S_IRWXU);
			if (ret == -1) {
				perror("Unable to create backup folder");
				free(backup_loc);
				return -1;
			}
		}

		int pos = -1;
		int num = 0;
		for (int i = strlen(oldpath) - 1; i >= 0; i--) {
			if (oldpath[i] == '/') {
				pos = i + 1;
				break;
			}
			num++;
		}

		char *fname = (char *)malloc((num + 1) * sizeof(char));
		if (pos == -1) {
			strncpy(fname, oldpath, num);
		} else {
			strncpy(fname, oldpath + pos, num);
		}
		fname[num] = '\0';

		unsigned long cur_timestamp = (unsigned long)time(NULL);
		snprintf(backup_loc + strlen(backup_loc), MAX_SIZE, "/%s_%lu", fname, cur_timestamp);
		free(fname);

		int fd_old = backup_open(oldpath, O_RDONLY, S_IRWXU);
		if (fd_old == -1) {
			free(backup_loc);
			return -1;
		}

		/*char err_buf[MAX_SIZE] = {0};
		snprintf(err_buf, MAX_SIZE, "Opened file %s for reading successfully", oldpath);
		perror(err_buf);*/

		int fd_new = backup_open(backup_loc, O_RDWR | O_CREAT, S_IRWXU);
		if (fd_new == -1) {
			close(fd_old);
			free(backup_loc);
			return -1;
		}

		/*snprintf(err_buf, MAX_SIZE, "Opened file %s for writing successfully", backup_loc);
		perror(err_buf);*/

		int nread = -1;
		char buf[MAX_SIZE] = {0};
		while ((nread = read(fd_old, buf, MAX_SIZE)) > 0) {
			int nwrite = write(fd_new, buf, nread);
			if (nwrite == -1) {
				nread = -1;
				break;
			}
		}

		free(backup_loc);
		close(fd_old);
		close(fd_new);

		if (nread == -1) {
			return -1;
		}
	}

	return backup_linkat(olddirfd, oldpath, newdirfd, newpath, flags);
}
int link(const char *oldpath, const char *newpath) {
	backup_link = dlsym(RTLD_NEXT, "link");

	if (!backup_open) {
		perror("Open not initialized");
		backup_open = dlsym(RTLD_NEXT, "open");
	}

	if (is_regular_file(oldpath)) {
		char *backup_loc = (char *)malloc(MAX_SIZE * sizeof(char));
		if (backup_loc == NULL) {
			return -1;
		}

		char *home_dir = getenv("HOME");
		snprintf(backup_loc, MAX_SIZE, "%s/.backup", home_dir);
		DIR *entry = opendir(backup_loc);
		if (entry == NULL) {
			int ret = mkdir(backup_loc, S_IRWXU);
			if (ret == -1) {
				perror("Unable to create backup folder");
				free(backup_loc);
				return -1;
			}
		}

		int pos = -1;
		int num = 0;
		for (int i = strlen(oldpath) - 1; i >= 0; i--) {
			if (oldpath[i] == '/') {
				pos = i + 1;
				break;
			}
			num++;
		}

		char *fname = (char *)malloc((num + 1) * sizeof(char));
		if (pos == -1) {
			strncpy(fname, oldpath, num);
		} else {
			strncpy(fname, oldpath + pos, num);
		}
		fname[num] = '\0';

		unsigned long cur_timestamp = (unsigned long)time(NULL);
		snprintf(backup_loc + strlen(backup_loc), MAX_SIZE, "/%s_%lu", fname, cur_timestamp);
		free(fname);

		int fd_old = backup_open(oldpath, O_RDONLY, S_IRWXU);
		if (fd_old == -1) {
			free(backup_loc);
			return -1;
		}

		/*char err_buf[MAX_SIZE] = {0};
		snprintf(err_buf, MAX_SIZE, "Opened file %s for reading successfully", oldpath);
		perror(err_buf);*/

		int fd_new = backup_open(backup_loc, O_RDWR | O_CREAT, S_IRWXU);
		if (fd_new == -1) {
			close(fd_old);
			free(backup_loc);
			return -1;
		}

		/*snprintf(err_buf, MAX_SIZE, "Opened file %s for writing successfully", backup_loc);
		perror(err_buf);*/

		int nread = -1;
		char buf[MAX_SIZE] = {0};
		while ((nread = read(fd_old, buf, MAX_SIZE)) > 0) {
			int nwrite = write(fd_new, buf, nread);
			if (nwrite == -1) {
				nread = -1;
				break;
			}
		}

		free(backup_loc);
		close(fd_old);
		close(fd_new);

		if (nread == -1) {
			return -1;
		}
	}

	return backup_link(oldpath, newpath);
}

int creat(const char *pathname, mode_t mode) {
	backup_creat = dlsym(RTLD_NEXT, "creat");

	if (!backup_open) {
		perror("Open not initialized");
		backup_open = dlsym(RTLD_NEXT, "open");
	}

	if (is_regular_file(pathname)) {
		char *backup_loc = (char *)malloc(MAX_SIZE * sizeof(char));
		if (backup_loc == NULL) {
			return -1;
		}

		char *home_dir = getenv("HOME");
		snprintf(backup_loc, MAX_SIZE, "%s/.backup", home_dir);
		DIR *entry = opendir(backup_loc);
		if (entry == NULL) {
			int ret = mkdir(backup_loc, S_IRWXU);
			if (ret == -1) {
				perror("Unable to create backup folder");
				free(backup_loc);
				return -1;
			}
		}

		int pos = -1;
		int num = 0;
		for (int i = strlen(pathname) - 1; i >= 0; i--) {
			if (pathname[i] == '/') {
				pos = i + 1;
				break;
			}
			num++;
		}

		char *fname = (char *)malloc((num + 1) * sizeof(char));
		if (pos == -1) {
			strncpy(fname, pathname, num);
		} else {
			strncpy(fname, pathname + pos, num);
		}
		fname[num] = '\0';

		unsigned long cur_timestamp = (unsigned long)time(NULL);
		snprintf(backup_loc + strlen(backup_loc), MAX_SIZE, "/%s_%lu", fname, cur_timestamp);
		free(fname);

		int fd_old = backup_open(pathname, O_RDONLY, S_IRWXU);
		if (fd_old == -1) {
			free(backup_loc);
			return -1;
		}

		/*char err_buf[MAX_SIZE] = {0};
		snprintf(err_buf, MAX_SIZE, "Opened file %s for reading successfully", pathname);
		perror(err_buf);*/

		int fd_new = backup_open(backup_loc, O_RDWR | O_CREAT, S_IRWXU);
		if (fd_new == -1) {
			close(fd_old);
			free(backup_loc);
			return -1;
		}

		/*snprintf(err_buf, MAX_SIZE, "Opened file %s for writing successfully", backup_loc);
		perror(err_buf);*/

		int nread = -1;
		char buf[MAX_SIZE] = {0};
		while ((nread = read(fd_old, buf, MAX_SIZE)) > 0) {
			int nwrite = write(fd_new, buf, nread);
			if (nwrite == -1) {
				nread = -1;
				break;
			}
		}


		free(backup_loc);
		close(fd_old);
		close(fd_new);
		
		if (nread == -1) {
			return -1;
		}

	}

	return backup_creat(pathname, mode);
}

int open(const char *pathname, int flags, ...) {
	backup_open = dlsym(RTLD_NEXT, "open");

	if ((flags & O_DIRECTORY) || check_file_exists(pathname) == false) {
		return backup_open(pathname, flags, S_IRWXU);
	}

	int fd = backup_open(pathname, flags, S_IRWXU);
	if (fd == -1) {
		return -1;
	}

	if (is_regular_file(pathname) && ((flags & O_WRONLY) || (flags & O_RDWR))) {
		char *backup_loc = (char *)malloc(MAX_SIZE * sizeof(char));
		if (backup_loc == NULL) {
			close(fd);
			return -1;
		}

		char *home_dir = getenv("HOME");
		snprintf(backup_loc, MAX_SIZE, "%s/.backup", home_dir);
		DIR *entry = opendir(backup_loc);
		if (entry == NULL) {
			int ret = mkdir(backup_loc, S_IRWXU);
			if (ret == -1) {
				perror("Unable to create backup folder");
				free(backup_loc);
				close(fd);
				return -1;
			}
		}

		int pos = -1;
		int num = 0;
		for (int i = strlen(pathname) - 1; i >= 0; i--) {
			if (pathname[i] == '/') {
				pos = i + 1;
				break;
			}
			num++;
		}

		char *fname = (char *)malloc((num + 1) * sizeof(char));
		if (pos == -1) {
			strncpy(fname, pathname, num);
		} else {
			strncpy(fname, pathname + pos, num);
		}
		fname[num] = '\0';

		unsigned long cur_timestamp = (unsigned long)time(NULL);
		snprintf(backup_loc + strlen(backup_loc), MAX_SIZE, "/%s_%lu", fname, cur_timestamp);
		free(fname);

		int fd_old = fd;
		if (flags & O_WRONLY) {
			fd_old = backup_open(pathname, O_RDONLY, S_IRWXU);
			if (fd_old == -1) {
				close(fd);
				free(backup_loc);
				return -1;
			}
		}

		/*char err_buf[MAX_SIZE] = {0};
		snprintf(err_buf, MAX_SIZE, "Opened file %s for reading successfully", pathname);
		perror(err_buf);*/

		int fd_new = backup_open(backup_loc, O_RDWR | O_CREAT, S_IRWXU);
		if (fd_new == -1) {
			close(fd_old);
			if (fd_old != fd) {
				close(fd);
			}
			free(backup_loc);
			return -1;
		}

		/*snprintf(err_buf, MAX_SIZE, "Opened file %s for writing successfully", backup_loc);
		perror(err_buf);*/

		int nread = -1;
		char buf[MAX_SIZE] = {0};
		while ((nread = read(fd_old, buf, MAX_SIZE)) > 0) {
			int nwrite = write(fd_new, buf, nread);
			if (nwrite == -1) {
				nread = -1;
				break;
			}
		}

		free(backup_loc);
		if (nread == -1) {
			close(fd_old);
			if (fd_old != fd) {
				close(fd);
			}
			close(fd_new);
			return -1;
		}

		if (fd_old != fd) {
			close(fd_old);
		}
		close(fd_new);
	}

	return fd;
}

int openat(int dirfd, const char *pathname, int flags, ...) {
	backup_openat = dlsym(RTLD_NEXT, "openat");

	if ((flags & O_DIRECTORY) || check_file_exists(pathname) == false) {
		return backup_openat(dirfd, pathname, flags, S_IRWXU);
	}

	int fd = backup_openat(dirfd, pathname, flags, S_IRWXU);
	if (fd == -1) {
		return -1;
	}

	if (is_regular_file(pathname) && ((flags & O_WRONLY) || (flags & O_RDWR))) {
		char *backup_loc = (char *)malloc(MAX_SIZE * sizeof(char));
		if (backup_loc == NULL) {
			close(fd);
			return -1;
		}

		char *home_dir = getenv("HOME");
		snprintf(backup_loc, MAX_SIZE, "%s/.backup", home_dir);
		DIR *entry = opendir(backup_loc);
		if (entry == NULL) {
			int ret = mkdir(backup_loc, S_IRWXU);
			if (ret == -1) {
				perror("Unable to create backup folder");
				free(backup_loc);
				close(fd);
				return -1;
			}
		}

		int pos = -1;
		int num = 0;
		for (int i = strlen(pathname) - 1; i >= 0; i--) {
			if (pathname[i] == '/') {
				pos = i + 1;
				break;
			}
			num++;
		}

		char *fname = (char *)malloc((num + 1) * sizeof(char));
		if (pos == -1) {
			strncpy(fname, pathname, num);
		} else {
			strncpy(fname, pathname + pos, num);
		}
		fname[num] = '\0';

		unsigned long cur_timestamp = (unsigned long)time(NULL);
		snprintf(backup_loc + strlen(backup_loc), MAX_SIZE, "/%s_%lu", fname, cur_timestamp);
		free(fname);

		int fd_old = fd;
		if (flags & O_WRONLY) {
			fd_old = backup_openat(dirfd, pathname, O_RDONLY, S_IRWXU);
			if (fd_old == -1) {
				close(fd);
				free(backup_loc);
				return -1;
			}
		}

		/*char err_buf[MAX_SIZE] = {0};
		snprintf(err_buf, MAX_SIZE, "Opened file %s for reading successfully", pathname);
		perror(err_buf);*/

		int fd_new = backup_openat(dirfd, backup_loc, O_RDWR | O_CREAT, S_IRWXU);
		if (fd_new == -1) {
			close(fd_old);
			if (fd_old != fd) {
				close(fd);
			}
			free(backup_loc);
			return -1;
		}

		/*snprintf(err_buf, MAX_SIZE, "Opened file %s for writing successfully", backup_loc);
		perror(err_buf);*/

		int nread = -1;
		char buf[MAX_SIZE] = {0};
		while ((nread = read(fd_old, buf, MAX_SIZE)) > 0) {
			int nwrite = write(fd_new, buf, nread);
			if (nwrite == -1) {
				nread = -1;
				break;
			}
		}

		free(backup_loc);
		if (nread == -1) {
			close(fd_old);
			if (fd_old != fd) {
				close(fd);
			}
			close(fd_new);
			return -1;
		}

		if (fd_old != fd) {
			close(fd_old);
		}
		close(fd_new);
	}

	return fd;
}
