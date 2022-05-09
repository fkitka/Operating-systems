CC = gcc
CFLAGS = -Wall -g

all:
# -lpthread is for program to work on ubuntu
	$(CC) $(CFLAGS) -o cook cook.c common.c -lpthread -lrt
	$(CC) $(CFLAGS) -o supplier supplier.c common.c -lpthread -lrt
	$(CC) $(CFLAGS) -o main main.c common.c -lpthread -lrt

clean:
	rm -f main cook supplier oven table

