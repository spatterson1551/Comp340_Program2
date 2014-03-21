#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

struct Process {
	int id;
	int arrive;
	int duration;
	int priority;
	struct Process *next;
};

struct cpuSlot {
	int procId;
	int startTime;
	int duration;
	int wait;
	int end;
	int turnaround;
	struct cpuSlot *next;
};

int totalTime(struct cpuSlot* root) {
	
	struct cpuSlot* current;
	int total = 0;
	current = root;
	while (current != 0) {
		total += current->duration;
		current = current->next;
	}
	
	return total;
}

struct cpuSlot* fcfs(struct Process* root, int quantum) {
	
	//set up pointer to traverse list
	struct Process* current;
	current = root;
	
	//set up list of slots
	struct cpuSlot* rootSlot;
	struct cpuSlot* currentSlot;
	rootSlot = malloc(sizeof(struct cpuSlot));
	rootSlot->next = 0;
	currentSlot = rootSlot;
	
	int prevDuration = 0;
	int prevStart = 0;
	
	
	// set the root
	currentSlot->procId = current->id;
	currentSlot->startTime = prevStart + prevDuration;
	currentSlot->duration = current->duration;
	currentSlot->wait = currentSlot->startTime - current->arrive; 
	currentSlot->end = currentSlot->startTime + currentSlot->duration;
	currentSlot->turnaround = (currentSlot->startTime + currentSlot->duration) - current->arrive;
	prevStart = currentSlot->startTime;
	prevDuration = currentSlot->duration;
	
	
	//now add each node to the list
	do {
		currentSlot->next = malloc(sizeof(struct cpuSlot));
		currentSlot = currentSlot->next;
		current = current->next;
		currentSlot->procId = current->id;
		currentSlot->startTime = prevStart + prevDuration;
		currentSlot->duration = current->duration;
		currentSlot->wait = currentSlot->startTime - current->arrive; 
		currentSlot->end = currentSlot->startTime + currentSlot->duration;
		currentSlot->turnaround = (currentSlot->startTime + currentSlot->duration) - current->arrive;
		prevStart = currentSlot->startTime;
		prevDuration = currentSlot->duration;
		currentSlot->next = 0;
	} while (current->next != 0);
	
	return rootSlot;
}
void rr(struct Process* root, int quantum) {
	
	//set up pointer to traverse list
	struct Process* current;
	current = root;
}
void priority_non(struct Process* root, int quantum) {

	//set up pointer to traverse list
	struct Process* current;
	current = root;
}
void priority(struct Process* root, int quantum) {

	//set up pointer to traverse list
	struct Process* current;
	current = root;
}

// Reads an arbitrarily long line from the file and returns it as a malloc-allocated buffer (caller is responsible for freeing it)
// There should not be any possibility of string buffer overflow, since the buffer is dynamically resized as necessary (think vector).
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
	free(policyLine);
		
	// Read the list of processes until we see a newline or eof
	char *nextProcessLine = readLineFromFile(fp);
	while(!feof(fp) && !ferror(fp) && strcmp(nextProcessLine, "\n") != 0)
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
		free(nextProcessLine);
		nextProcessLine = readLineFromFile(fp);
	}
	
	//print out the linked list of processes, simply for testing purposes at this point, but
	//it could be nice just to give the user some more feedback anyways
	printf("\nThe schedule policy is as follows: %s \n", policy);
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
	}
	
	int wait = 0;
	int turnaround = 0;
	
	if (strcmp(policy, "fcfs") == 0) {
		struct cpuSlot* slots;
		slots = fcfs(rootProcess, quantum);
		printf("%d ", totalTime(slots));  //for fcfs busy time and total are the same.
		printf("%d \n", totalTime(slots));
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
		rr(rootProcess, quantum);
	} else if (strcmp(policy, "priority_non") == 0) {
		priority_non(rootProcess, quantum);
	} else if (strcmp(policy, "priority") == 0) {
		priority(rootProcess, quantum);
	} else {
		printf("The schedule policy in the input file is invalid\n");
	}

	// TODO: traverse the process list and free memory
	
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
