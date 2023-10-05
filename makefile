CC = gcc
CFLAGS = -Wall -mwindows
LIBS = -lws2_32

all: device.c
	$(CC) $(CFLAGS) -o device device.c $(LIBS)

install: oneclickinstall.c
	$(CC) -o oneclickinstall oneclickinstall.c