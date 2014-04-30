/*****************

AUTHORS: Steve Patterson, Ethan Johnson
CLASS: COMP 340
DATE: 4/30/13
DESCRIPTION: Test file to show what happens in memory
when the parent forks 3 children.
NOTE: This is only to be run from another program using execv
COMPILE: gcc -o execvTest execvTest.c
RUN: free; ./execvTest

*****************/

#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main() {

    pid_t pid[3];
	//char* buffer = (char*)malloc(sizeof(char)*10000000);
    printf("before any allocations memory is: \n");
	system("free");

    if ((pid[0] = fork()) == -1) //fork the first child
        perror("fork error");
    else if (pid[0] == 0) { //this is child 1
		printf("After child1 process is created\n");
		system("free");
		return;
    } else { //this is the parent
		wait(NULL); // wait for first child to terminate
		if (pid[1] = fork() == -1) {   //now fork the second child
			perror("fork error");
		} else if (pid[1] == 0) { //this is child 2
			printf("After child2 process is created\n");
			system("free");
			return;
		} else { // this is the parent
			wait(NULL); // wait for the second child to terminate
			if (pid[2] = fork() == -1) {  //now fork the third child
				perror("fork error");
			} else if (pid[2] == 0) { // this is child 3
				printf("After child3 process is created\n");
				system("free");
				return;
			} else { //this is the parent
				wait(NULL); // wait for final child to terminate
			}
		}
	}
	
	
    
    return 0;
}

