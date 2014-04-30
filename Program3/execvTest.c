/*****************

AUTHORS: Steve Patterson, Ethan Johnson
CLASS: COMP 340
DATE: 4/30/13
DESCRIPTION: Test process to be called
by another process using execv in order to 
demonstrate memory usage
NOTE: This is only to be run from another program using execv
COMPILE: "gcc -o execvTest execvTest.c"
RUN: "./execvTest", see NOTE above

*****************/


#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>

int main() {

    char* dummy = (char*)malloc(sizeof(char)*10000000);
    printf("after the execv call\n");
	system("free");
	memset(dummy, 0, 10000000);
	printf("After the execv called process allocates memory\n");
	system("free");

	return 0;
}