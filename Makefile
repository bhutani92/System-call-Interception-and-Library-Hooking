OBJS = main.o
CC = gcc
CFLAGS = -Wall -Werror -Wno-deprecated-declarations
LFLAGS = -fPIC -shared -ldl -D_GNU_SOURCE

helloWorld: 	helloWorld.c
		$(CC) -o helloWorld.out helloWorld.c

backupFiles: 	backupFiles.c
		$(CC) -o backupFiles.so $(LFLAGS)

all:		helloWorld backupFiles

clean :
	-@rm *.out *.so *.tar.gz 2>/dev/null || true
