OBJS = main.o
CC = gcc
CFLAGS = -Wall -Werror -Wno-deprecated-declarations
LFLAGS = -fPIC -shared -ldl -D_GNU_SOURCE

backupFiles: 	backupFiles.c
		$(CC) $(CFLAGS) backupFiles.c -o backupFiles.so $(LFLAGS)

test: 		helloWorld.c
		$(CC) helloWorld.c $(CFLAGS) -o helloWorld.out helloWorld.c

all:		backupFiles

tar:		
	tar zcvf AkhilBhutani_110898687.tar.gz Makefile backupFiles.c CSE_509_Assignment_3.pdf helloWorld.c README.md backupFiles.so helloWorld

clean :
	-@rm *.out *.so *.tar.gz 2>/dev/null || true
