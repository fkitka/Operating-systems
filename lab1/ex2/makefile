CC=gcc
CFLAGS=-c -Wall

all: run_tests

static:
	$(CC) $(CFLAGS) library.c
	ar rcs liblibrary.a library.o

shared:
	$(CC) $(CFLAGS) -fPIC library.c
	$(CC) -shared -fPIC -o liblibrary.so library.o -lc
clean:
	rm -f *.o
	rm -f *.so
	rm -f *.a
	rm main

main_static: static
	$(CC) $(CFLAGS) main.c
	$(CC) main.o liblibrary.a -o main

main_shared: shared
	$(CC) main.c -l library -L ./ -o main

run_tests:
	rm -f raport2.txt
	make tests_static

tests_static: main_static
	@echo "------STATIC TEST------" >> raport2.txt
	make --no-print-directory small
	make --no-print-directory medium
	make --no-print-directory large

small:
	python3 gen.py 10 10 10 file
	@echo "------------SMALL TESTS------------" >> raport2.txt
	@./main create_table 10 wc_files file1 file2 file3 file4 wc_files file3 file5 file6 file7 file8 file2 wc_files file9 file0 remove_block 1 remove_block 0
	@
	@./main create_table 10 wc_files file1 file2 remove_block 0 wc_files file2 file3 file4 wc_files file5 file7 file8 remove_block 0 wc_files file5 remove_block 1 wc_files file8 wc_files file0 remove_block 0

medium:
	python3 gen.py 10 1000 100 file
	@echo "------------MEDIUM TESTS------------" >> raport2.txt
	@./main create_table 30 wc_files file1 file2 file3 file4 wc_files file3 file5 file6 file7 file8 file2 wc_files file9 file0 wc_files file1 file2 file3 file8 file9 wc_files file5 file6 file7 file8 file1 file2 file4 file3 file5 remove_block 1 remove_block 0
	@
	@./main create_table 30 wc_files file1 file2 wc_files file3 file4 wc_files file3 file5 remove_block 0 wc_files file6 file7 file8 file2 remove_block 1 wc_files file1 file2 file3 file4 file5 file6 file7 remove_block 1 wc_files file9 file0 remove_block 0

large:
	python3 gen.py 10 10000 110 file
	@echo "------------LARGE TESTS------------" >> raport2.txt
	@./main create_table 100 wc_files file1 file2 wc_files file3 file4 wc_files file3 file5 remove_block 0 wc_files file6 file7 file8 file2 remove_block 2 wc_files file1 file2 file3 wc_files file4 file5 file6 file7 remove_block 3 wc_files file9 file0 wc_files file1 file2 file3 file8 file9 wc_files file5 file6 file7 file8 file9 file0 file1 file2 file4 file3 file5 remove_block 1
	@
	@./main create_table 100 wc_files file1 file2 wc_files file3 file4 wc_files file1 file2 file3 file8 file9 wc_files file5 file6 file7 file8 file9 file0 file1 file2 file4 file3 file5 wc_files file3 file5 remove_block 0 wc_files file6 file7 file8 file2 remove_block 2 wc_files file1 file2 file3 file4 file5 file6 file7 remove_block 3 wc_files file9 file0 remove_block 1

