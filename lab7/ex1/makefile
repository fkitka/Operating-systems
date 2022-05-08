CC = gcc
CFLAGS = -Wall

all:
	touch oven
	touch table
	$(CC) $(FLAGS) -o cook cook.c common.c
	$(CC) $(FLAGS) -o supplier supplier.c common.c
	$(CC) $(FLAGS) -o main main.c common.c

clean:
	rm -f main cook supplier oven table