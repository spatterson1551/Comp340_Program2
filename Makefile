# To run, enter "make all" or simply "make" (which always executes the first target in the Makefile, in our case "all")

all: scheduler

clean:
	rm *.o *.exe

scheduler: main.o
	gcc -o scheduler main.o -std=c99

main.o: main.c
	gcc -c main.c -std=c99
