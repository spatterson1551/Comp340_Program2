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

void fcfs(struct Process* root, int quantum) {
	
	//set up pointer to traverse list
	struct Process* current;
	current = root;
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

int main() {

	
	//init the root node of the Process list
	struct Process* rootProcess;
	rootProcess = malloc(sizeof(struct Process));
	rootProcess->next = 0;
	
	//init the Process pointer which will traverse the list
	struct Process* current;
	current = rootProcess;
	
	//init variables to hold information about process list
	char policy[15];
	int quantum;
	
	//init the file stream
	FILE *fp;
	fp = fopen("input.txt", "r");
	
	//read in the policy and quantum from the file
	fscanf(fp, "%s", policy);
	fscanf(fp, "%d", &quantum);
	
	//now build the linked list of processes by going through the file
	//set the root
	fscanf(fp, "%d", &current->id);
	fscanf(fp, "%d", &current->arrive);
	fscanf(fp, "%d", &current->duration);
	fscanf(fp, "%d", &current->priority);
	
	//now set all the following Process nodes
	//NOTE: only works when there is no \n character after the last line in the txt file
	do { 
		current->next = malloc( sizeof(struct Process) ); 
		current = current->next;
		fscanf(fp, "%d", &current->id);
		fscanf(fp, "%d", &current->arrive);
		fscanf(fp, "%d", &current->duration);
		fscanf(fp, "%d", &current->priority);
		current->next = 0;
	} while(!feof(fp));
	
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
	
	
	if (strcmp(policy, "fcfs") == 0) {
		fcfs(rootProcess, quantum);
	} else if (strcmp(policy, "rr") == 0) {
		rr(rootProcess, quantum);
	} else if (strcmp(policy, "priority_non") == 0) {
		priority_non(rootProcess, quantum);
	} else if (strcmp(policy, "priority") == 0) {
		priority(rootProcess, quantum);
	}
	
	return 0;
}


