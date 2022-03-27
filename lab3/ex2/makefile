all: compile

compile:
	gcc -Wall -o main main.c
test: compile
	./main 0.001 10
	./main 0.00001 10
	./main 0.0001 100
	./main 0.000001 1000
	./main 0.0000001 1000
	./main 0.00000001 1000
	./main 0.000000001 1000
	./main 0.0000000001 1000
	./main 0.0000001 5000
	./main 0.00000001 5000
	./main 0.0000001 10000
	./main 0.00000001 10000

	make clean
clean:
	rm -f *.o w*.txt main