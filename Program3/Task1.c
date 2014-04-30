/*****************

AUTHORS: Steve Patterson, Ethan Johnson
CLASS: COMP 340
DATE: 4/30/13
DESCRIPTION: Test program to see demonstrate
memory allocation when different sized processes are called
COMPILE: gcc -o Task1 Task1.c
RUN: free; ./Task1

*****************/

#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main() {

    pid_t pid[2];
	char* dummy = (char*)malloc(sizeof(char)*10000000); // set up dummy to be used later
	char* dummy2 = (char*)malloc(sizeof(char)*100000000); //set up larger dummy to be used later
    printf("before any allocations memory is: \n");
	system("free");

    if ((pid[0] = fork()) == -1)
        perror("fork error");
    else if (pid[0] == 0) { //child
		printf("After child 1 process but before it allocates memory\n");
		system("free");
		memset(dummy, 0, 10000000); //actually sets the memory, else none will be allocated
		printf("after the allocation of memory by child process 1 \n");
		system("free");
		return;
    } else { //parent
		wait(NULL); // wait for first child to finish
		if ((pid[1] = fork()) == -1) { // fork another child which uses more memory
			perror("fork error");
		} else if (pid[1] == 0) { // second child
			printf("After child 2 process but before it allocates memory\n");
			system("free");
			memset(dummy2, 0, 100000000); //actually sets the memory, else none will be allocated
			printf("after the allocation of memory by child process 2 \n");
			system("free");
			sleep(10); //sleep for a while to see if memory allocation to a process changes over time
			printf("10 seconds later, memory usage for child process 2 is now \n");
			system("free");
		} else { // parent
			wait(NULL); //wait for second child to terminate
		}
	}
	
	
    
    return 0;
}