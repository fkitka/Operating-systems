CC=gcc
CFLAGS=-Wall
OP1=-O
OP2=-O1
OP3=-O2

all: run_tests

static:
	$(CC) $(CFLAGS) -c library.c
	ar rcs liblibrary.a library.o

shared:
	$(CC) $(CFLAGS) -c -fPIC library.c
	$(CC) -shared -o liblibrary.so library.o -lc
clean:
	rm -f *.o *.so *.a main

main_static: static
	$(CC) $(CFLAGS) -c main.c
	$(CC) main.o liblibrary.a -o main

main_shared: shared
	$(CC) $(CFlags) main.c -llibrary -L ./ -o main
	#export LD_LIBRARY_PATH=$(PWD)

main_dynamic: shared
	$(CC) $(CFLAGS) main.c -ldl -D DYNAMIC -o main

run_tests:
	rm -f results3b.txt
	make tests_static
	make tests_shared
	make tests_dynamic

tests_static:
	$(CC) $(CFLAGS) $(OP1) -c library.c
	ar rcs liblibrary.a library.o
	$(CC) $(CFLAGS) $(OP1) -c main.c
	$(CC) main.o liblibrary.a -o main

	@echo "OPTIMALISATION $(OP1)" >> results3b.txt
	@echo "------STATIC TEST------" >> results3b.txt
	make --no-print-directory small
	make --no-print-directory medium
	make --no-print-directory large

#---------------

	$(CC) $(CFLAGS) $(OP2) -c library.c
	ar rcs liblibrary.a library.o
	$(CC) $(CFLAGS) $(OP2) -c main.c
	$(CC) main.o liblibrary.a -o main

	@echo "OPTIMALISATION $(OP2)" >> results3b.txt
	@echo "------STATIC TEST------" >> results3b.txt
	make --no-print-directory small
	make --no-print-directory medium
	make --no-print-directory large

#--------------

	$(CC) $(CFLAGS) $(OP3) -c library.c
	ar rcs liblibrary.a library.o
	$(CC) $(CFLAGS) $(OP3) -c main.c
	$(CC) main.o liblibrary.a -o main

	@echo "OPTIMALISATION $(OP3)" >> results3b.txt
	@echo "------STATIC TEST------" >> results3b.txt
	make --no-print-directory small
	make --no-print-directory medium
	make --no-print-directory large

tests_shared:
	$(CC) $(CFLAGS) $(OP1) -c -fPIC library.c
	$(CC) -shared -o liblibrary.so library.o -lc
	$(CC) $(CFlags) $(OP1) main.c -llibrary -L ./ -o main

	@echo "OPTIMALISATION $(OP1)" >> results3b.txt
	@echo "------SHARED TEST------" >> results3b.txt
	make --no-print-directory small
	make --no-print-directory medium
	make  --no-print-directory large

#--------------

	$(CC) $(CFLAGS) $(OP2) -c -fPIC library.c
	$(CC) -shared -o liblibrary.so library.o -lc
	$(CC) $(CFlags) $(OP2) main.c -llibrary -L ./ -o main

	@echo "OPTIMALISATION $(OP2)" >> results3b.txt
	@echo "------SHARED TEST------" >> results3b.txt
	make --no-print-directory small
	make --no-print-directory medium
	make  --no-print-directory large

#-------------

	$(CC) $(CFLAGS) $(OP3) -c -fPIC library.c
	$(CC) -shared -o liblibrary.so library.o -lc
	$(CC) $(CFlags) $(OP3) main.c -llibrary -L ./ -o main

	@echo "OPTIMALISATION $(OP3)" >> results3b.txt
	@echo "------SHARED TEST------" >> results3b.txt
	make --no-print-directory small
	make --no-print-directory medium
	make  --no-print-directory large

tests_dynamic:

	$(CC) $(CFLAGS) $(OP1) -c -fPIC library.c
	$(CC) -shared -o liblibrary.so library.o -lc
	$(CC) $(CFLAGS) $(OP1) main.c -ldl -D DYNAMIC -o main

	@echo "OPTIMALISATION $(OP1)" >> results3b.txt
	@echo "------DYNAMIC TEST------" >> results3b.txt
	make --no-print-directory small
	make --no-print-directory medium
	make  --no-print-directory large

#---------

	$(CC) $(CFLAGS) $(OP2) -c -fPIC library.c
	$(CC) -shared -o liblibrary.so library.o -lc
	$(CC) $(CFLAGS) $(OP2) main.c -ldl -D DYNAMIC -o main

	@echo "OPTIMALISATION $(OP2)" >> results3b.txt
	@echo "------DYNAMIC TEST------" >> results3b.txt
	make --no-print-directory small
	make --no-print-directory medium
	make  --no-print-directory large

#--------

	$(CC) $(CFLAGS) $(OP3) -c -fPIC library.c
	$(CC) -shared -o liblibrary.so library.o -lc
	$(CC) $(CFLAGS) $(OP3) main.c -ldl -D DYNAMIC -o main

	@echo "OPTIMALISATION $(OP3)" >> results3b.txt
	@echo "------DYNAMIC TEST------" >> results3b.txt
	make --no-print-directory small
	make --no-print-directory medium
	make  --no-print-directory large


small:
	python3 gen.py 10 10 10 file
	@echo "------------SMALL TESTS------------" >> results3b.txt
	@./main create_table 10 wc_files file1 file2 file3 file4 wc_files file3 file5 file6 file7 file8 file2 wc_files file9 file0 remove_block 1 remove_block 0
	@
	@./main create_table 10 wc_files file1 file2 remove_block 0 wc_files file2 file3 file4 wc_files file5 file7 file8 remove_block 0 wc_files file5 remove_block 1 wc_files file8 wc_files file0 remove_block 0

medium:
	
	python3 gen.py 10 1000 100 file
	@echo "------------MEDIUM TESTS------------" >> results3b.txt
	@./main create_table 30 wc_files file1 file2 file3 file4 wc_files file3 file5 file6 file7 file8 file2 wc_files file9 file0 wc_files file1 file2 file3 file8 file9 wc_files file5 file6 file7 file8 file1 file2 file4 file3 file5 remove_block 1 remove_block 0
	@
	@./main create_table 30 wc_files file1 file2 wc_files file3 file4 wc_files file3 file5 remove_block 0 wc_files file6 file7 file8 file2 remove_block 1 wc_files file1 file2 file3 file4 file5 file6 file7 remove_block 1 wc_files file9 file0 remove_block 0

large:
	
	python3 gen.py 10 10000 110 file
	@echo "------------LARGE TESTS------------" >> results3b.txt
	@./main create_table 100 wc_files file1 file2 wc_files file3 file4 wc_files file3 file5 remove_block 0 wc_files file6 file7 file8 file2 remove_block 2 wc_files file1 file2 file3 wc_files file4 file5 file6 file7 remove_block 3 wc_files file9 file0 wc_files file1 file2 file3 file8 file9 wc_files file5 file6 file7 file8 file9 file0 file1 file2 file4 file3 file5 remove_block 1
	@
	@./main create_table 100 wc_files file1 file2 wc_files file3 file4 wc_files file1 file2 file3 file8 file9 wc_files file5 file6 file7 file8 file9 file0 file1 file2 file4 file3 file5 wc_files file3 file5 remove_block 0 wc_files file6 file7 file8 file2 remove_block 2 wc_files file1 file2 file3 file4 file5 file6 file7 remove_block 3 wc_files file9 file0 remove_block 1

