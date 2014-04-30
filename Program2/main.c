/*
  Authors: Ethan Johnson, Steve Patterson
  Course:  COMP 340, Operating Systems
  Date:    21 March 2014
  Description:	 Main driver functionality for Program 2
  Compile with:  make
  Run with:      ./scheduler
*/

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>

#include "schedulers.h"

// Reads an arbitrarily long line from a file and returns it as a malloc-allocated buffer (caller is responsible for freeing it)
// There should not be any possibility of string buffer overflow, since the buffer is dynamically resized as necessary (think std::vector).
// (Imported from Ethan's Program 1 code.)
char* readLineFromFile(FILE *fp);

int main() {
	//init the root node of the Process list
	struct Process* rootProcess = 0;
	
	//init the Process pointer which will traverse the list
	struct Process* current = 0;
	
	//init variables to hold information about process list
	char policy[15];
	int quantum;
	
	//init the file stream
	FILE *fp;
	fp = fopen("input.txt", "r");
	
	// Read the input file and create the list of Process nodes
	char *policyLine = readLineFromFile(fp);
	sscanf(policyLine, "%s %d", policy, &quantum);
	free(policyLine); policyLine = 0;
		
	// Read the list of processes until we see a newline or eof
	char *nextProcessLine = readLineFromFile(fp);
	while(strcmp(nextProcessLine, "\n") != 0)
	{
		int id, arrive, duration, priority;
		if(sscanf(nextProcessLine, "%d %d %d %d", &id, &arrive, &duration, &priority) != 4)
		{
			// If sscanf returned something other than 4, we didn't get a complete line - skip this process
			fprintf(stderr, "Error: process line incomplete, skipping it.\n");
		}
		else
		{
			// We got a complete line - build a Process struct and add it to the list
			if(current == 0) // this is the root node
			{
				rootProcess = malloc(sizeof(struct Process));
				current = rootProcess;
			}
			else
			{
				current->next = malloc(sizeof(struct Process));
				current = current->next;
			}

			current->id = id;
			current->arrive = arrive;
			current->duration = duration;
			current->priority = priority;
			current->next = 0;
		}

		// Read the next line
		free(nextProcessLine); nextProcessLine = 0;
		if(feof(fp) || ferror(fp))
			break;
		nextProcessLine = readLineFromFile(fp);
	}
	if(nextProcessLine)
		free(nextProcessLine); nextProcessLine = 0;

	// If there were no processes in the file, proceed no further.
	if(current == 0)
	{
		fprintf(stderr, "No processes specified - exiting.\n");
		exit(1);
	}
	
	// Print out the list of processes -- for debugging
	// (Uncomment from here to the end of the while-loop if you want a little more verbose output.)
	/*printf("\nThe schedule policy is as follows: %s \n", policy);
	printf("The quantum for the policy is: %d \n", quantum);
	printf("The input file has provided these processes:\n");
	current = rootProcess;
	while (current != 0) {
		printf("P%d: ", current->id);
		printf( "id: %d ", current->id);
		printf( "arrive: %d ", current->arrive);
		printf( "duration: %d ", current->duration);
		printf( "priority: %d \n", current->priority);
		current = current->next;
	}*/
	
	int wait = 0;
	int turnaround = 0;
	int downTime = 0;

	struct cpuSlot *slots;
	
	if (strcmp(policy, "fcfs") == 0) {
		slots = fcfs(rootProcess, quantum, &downTime);
		printf("%d ", totalTime(slots));  
		printf("%d \n", totalTime(slots) + downTime);
		int count = 0;
		while (slots != 0) {
			printf("%d ", slots->procId);
			printf( "%d ", slots->startTime);
			printf( "%d ", slots->end);
			printf( "%d ", slots->turnaround);
			turnaround += slots->turnaround;
			printf( "%d \n", slots->wait);
			wait += slots->wait;
			slots = slots->next;
			count++;
		}
		printf("%f ", (float)turnaround/(float)count);
		printf("%f \n", (float)wait/(float)count);
	} else if (strcmp(policy, "rr") == 0) {
		slots = rr(rootProcess, quantum, &downTime);
		printf("%d ", totalTime(slots));
		printf("%d \n", totalTime(slots) + downTime);
		int count = 0;
		while (slots != 0) {
			printf("%d ", slots->procId);
			printf("%d ", slots->startTime);
			printf("%d ", slots->end);
			printf("%d ", slots->turnaround);
			turnaround += slots->turnaround;
			printf( "%d \n", slots->wait);
			wait += slots->wait;
			slots = slots->next;
			count++;
		}
		printf("%f ", (float)turnaround/(float)count);
		printf("%f \n", (float)wait/(float)count);
	} else if (strcmp(policy, "priority_non") == 0) {
		slots = priority_non(rootProcess, quantum, &downTime);
		printf("%d ", totalTime(slots));  
		printf("%d \n", totalTime(slots) + downTime);
		int count = 0;
		while (slots != 0) {
			printf("%d ", slots->procId);
			printf( "%d ", slots->startTime);
			printf( "%d ", slots->end);
			printf( "%d ", slots->turnaround);
			turnaround += slots->turnaround;
			printf( "%d \n", slots->wait);
			wait += slots->wait;
			slots = slots->next;
			count++;
		}
		printf("%f ", (float)turnaround/(float)count);
		printf("%f \n", (float)wait/(float)count);
	} else if (strcmp(policy, "priority") == 0) {
		slots = priority(rootProcess, quantum, &downTime);
		printf("%d ", totalTime(slots));  
		printf("%d \n", totalTime(slots) + downTime);
		int count = 0;
		while (slots != 0) {
			printf("%d ", slots->procId);
			printf( "%d ", slots->startTime);
			printf( "%d ", slots->end);
			printf( "%d ", slots->turnaround);
			turnaround += slots->turnaround;
			printf( "%d \n", slots->wait);
			wait += slots->wait;
			slots = slots->next;
			count++;
		}
		printf("%f ", (float)turnaround/(float)count);
		printf("%f \n", (float)wait/(float)count);
	} else {
		printf("The schedule policy in the input file is invalid\n");
	}

	// Deallocate the process and CPU-slot lists
	for(struct Process *p = rootProcess; p != 0; )
	{
		struct Process *toDelete = p;
		p = p->next;
		free(toDelete);
	}
	rootProcess = 0;
	for(struct cpuSlot *p = slots; p != 0; )
	{
		struct cpuSlot *toDelete = p;
		p = p->next;
		free(toDelete);
	}
	slots = 0;
	
	return 0;
}

char* readLineFromFile(FILE *fp)
{
	const int BUFFER_SIZE = 255;
	char *buffer = malloc(BUFFER_SIZE * sizeof(char));

	int timesRead = 1;
	fgets(buffer, BUFFER_SIZE, fp);
	while(strlen(buffer) == ((BUFFER_SIZE-1) * timesRead)) // we didn't get the whole line
	{
		int continuePoint = (BUFFER_SIZE-1) * timesRead;
		buffer = realloc(buffer, (continuePoint + BUFFER_SIZE) * sizeof(char));
		fgets(buffer + continuePoint, BUFFER_SIZE, fp);
		timesRead++;
	}

	return buffer;
}
