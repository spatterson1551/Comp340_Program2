/*****************

AUTHORS: Steve Patterson, Ethan Johnson
CLASS: COMP 340
DATE: 4/30/13
DESCRIPTION: Program which creates a child that calls
execv to run another process. Used to demonstrate how 
memory is allocated for the child and the process it calls.
NOTE: THIS IS ONLY TO BE RUN AFTER execvTest.c HAS BEEN COMPILED
COMPILE: "gcc -o Task2b Task2b.c"
RUN: "free; ./Task2b"    (free; is called first to show memory before the program itself is run)

*****************/

#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>

int main() {

    pid_t pid;  //create a process id for the child
                    
    char *const cmd[] = {"./execvTest", NULL}; //set up the second argument to the execv function

    if ((pid = fork()) == -1)    //fork, if pid == -1, then there was an error
        perror("fork error");
    else if (pid == 0) {  //this is the child 
		printf("memory before execv call \n"); //output the memory before the execv call
		system("free");
        execv("./execvTest", cmd); // call execv and load a dummy program that simply allocates memory
    } else { //this is the parent
		wait(NULL);    // wait for child
		printf("final memory usage \n"); //show memory after all processes have run their course
		system("free");
	}

	return 0;
}
