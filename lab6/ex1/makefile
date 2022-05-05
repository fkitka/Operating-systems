CC=gcc
CFlags=-Wall

all: server client


common:
	$(CC) $(CFlags) -c common.c
	ar rcs libcommon.a common.o


client: common
	$(CC) $(CFlags) -o client client.c libcommon.a


server: common
	$(CC) $(CFlags) -o server server.c libcommon.a


clean:
	rm -rf server client libcommon.a common.o