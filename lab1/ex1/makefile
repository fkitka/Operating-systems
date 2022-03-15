CC=gcc
CFLAGS=-c -Wall

all: static shared

static:
	$(CC) $(CFLAGS) library.c
	ar rcs liblibrary.a library.o

shared:
	$(CC) $(CFLAGS) -fPIC library.c
	$(CC) -shared -o liblibrary.so library.o -lc

clean:
	rm -f *.o *.so *.a
