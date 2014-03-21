# To run, enter "make all" or simply "make" (which always executes the first target in the Makefile, in our case "all")

all: scheduler

clean:
	rm *.o *.exe

scheduler: main.o schedulers.o
	gcc -o scheduler main.o schedulers.o -std=c99 -ggdb

main.o: main.c schedulers.h
	gcc -c main.c -std=c99 -ggdb

schedulers.o: schedulers.c schedulers.h
	gcc -c schedulers.c -std=c99 -ggdb