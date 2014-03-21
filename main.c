#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>

struct Process {
	int id;
	int arrive;
	int duration;
	int priority;
	struct Process *next;
};

// Used to keep track of processes during sorting
struct Process_sort {
	struct Process *proc;
	bool selected;
	struct Process_sort *next;
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

struct cpuSlot* fcfs(struct Process* root, int quantum, int* downTime) {

	//set up pointer to traverse list
	struct Process* current;
	
	//set up list of slots
	struct cpuSlot* rootSlot;
	struct cpuSlot* currentSlot;
	rootSlot = malloc(sizeof(struct cpuSlot));
	rootSlot->next = 0;
	currentSlot = rootSlot;
	
	int prevDuration = 0;
	int prevStart = 0;

	// Sort the list of processes by arrival time (with PID as second criteria, but we don't need
	// to handle that explicitly since the input is guaranteed to be ordered by increasing PID).

	// Create a Process_sort linked list to keep track of the processs during sorting
	current = root;
	struct Process_sort *sortHead = malloc(sizeof(struct Process_sort));
	sortHead->proc = root;
	sortHead->selected = false;
	sortHead->next = 0;
	struct Process_sort *currentSort = sortHead;
	while(current->next != 0)
	{
		current = current->next;

		currentSort->next = malloc(sizeof(struct Process_sort));
		currentSort = currentSort->next;

		currentSort->proc = current;
		currentSort->selected = false;
		currentSort->next = 0;
	}

	// Sort the processes using an insertion sort
	struct Process sortedListHead; // dummy node at the head of the new sorted list to make things easier
	current = &sortedListHead;
	while(true)
	{
		// Traverse the list to find the least node that we haven't yet selected (put into place in the sorted list)
		struct Process_sort *least = 0;
		
		currentSort = sortHead;
		while(currentSort != 0)
		{
			if(!currentSort->selected) // we haven't yet put this element into place in the sorted list
			{
				if(least == 0 || currentSort->proc->arrive < least->proc->arrive) // we've found a new least
				{
					// Unselect the old least (if we have one)
					if(least)
						least->selected = false;
					least = currentSort;
					// Select the new one
					least->selected = true;
				}
			}

			currentSort = currentSort->next;
		}

		if(least) // If we found an unselected node
		{
			// Add the just-found next-least node to the new, sorted list
			// (note that in the process, we're destroying the original non-sorted list - that's
			// OK, since we have pointers to all nodes saved in the Process_sort list and are thus
			// going to reconstruct the full list in the sorting process)
			current->next = least->proc;
			current->next->next = 0;
			current = current->next;
		}
		else // all nodes are selected, so sorting is done
			break;
	}
	// Point root to the head of the new sorted list
	root = sortedListHead.next;

	// Deallocate the Process_sort list (no longer needed since sorting is complete)
	for(struct Process_sort *p = sortHead; p->next != 0; )
	{
		struct Process_sort *toDelete = p->next;
		p->next = p->next->next;
		free(toDelete);
	}
	free(sortHead); sortHead = 0;

	// The processes are now sorted; proceed with scheduling.
	current = root;
	
	// set the root
	currentSlot->procId = current->id;
	if ((prevStart + prevDuration) < current->arrive) {
		currentSlot->startTime = current->arrive;
	} else {
		currentSlot->startTime = prevStart + prevDuration;
	}
	currentSlot->duration = current->duration;
	currentSlot->wait = currentSlot->startTime - current->arrive; 
	currentSlot->end = currentSlot->startTime + currentSlot->duration;
	currentSlot->turnaround = (currentSlot->startTime + currentSlot->duration) - current->arrive;
	if (current->arrive > (prevStart + prevDuration)) {
		*downTime += current->arrive - (prevStart + prevDuration);
	}
	prevStart = currentSlot->startTime;
	prevDuration = currentSlot->duration;
	
	
	//now add each node to the list
	while (current->next != 0) {
		currentSlot->next = malloc(sizeof(struct cpuSlot));
		currentSlot = currentSlot->next;
		current = current->next;

		currentSlot->procId = current->id;
		if ((prevStart + prevDuration) < current->arrive) {
			currentSlot->startTime = current->arrive;
		} else {
			currentSlot->startTime = prevStart + prevDuration;
		}
		currentSlot->duration = current->duration;
		currentSlot->wait = currentSlot->startTime - current->arrive; 
		currentSlot->end = currentSlot->startTime + currentSlot->duration;
		currentSlot->turnaround = (currentSlot->startTime + currentSlot->duration) - current->arrive;
		if (current->arrive > (prevStart + prevDuration)) {
			*downTime += current->arrive - (prevStart + prevDuration);
		}
		prevStart = currentSlot->startTime;
		prevDuration = currentSlot->duration;
		currentSlot->next = 0;
	}
	
	return rootSlot;
}
void rr(struct Process* root, int quantum) {
	
	//set up pointer to traverse list
	struct Process* current;
	current = root;
}
struct cpuSlot* priority_non(struct Process* root, int quantum, int* downTime) {
	
	// Sort the list of processes by priority first, arrival time second, and PID third (all lowest-first)
	// (note: they're already sorted by PID, so we don't need to sort explicitly on that)

	// Create a Process_sort linked list to keep track of the processs during sorting
	struct Process *current = root;
	struct Process_sort *sortHead = malloc(sizeof(struct Process_sort));
	sortHead->proc = root;
	sortHead->selected = false;
	sortHead->next = 0;
	struct Process_sort *currentSort = sortHead;
	while(current->next != 0)
	{
		current = current->next;

		currentSort->next = malloc(sizeof(struct Process_sort));
		currentSort = currentSort->next;

		currentSort->proc = current;
		currentSort->selected = false;
		currentSort->next = 0;
	}

	// Sort the processes using an insertion sort
	struct Process sortedListHead; // dummy node at the head of the new sorted list to make things easier
	current = &sortedListHead;
	while(true)
	{
		// Traverse the list to find the least node that we haven't yet selected (put into place in the sorted list)
		struct Process_sort *least = 0;
		
		currentSort = sortHead;
		while(currentSort != 0)
		{
			if(!currentSort->selected) // we haven't yet put this element into place in the sorted list
			{
				if(least == 0 || 
					(currentSort->proc->priority < least->proc->priority) ||
					(currentSort->proc->priority == least->proc->priority && currentSort->proc->arrive < least->proc->arrive)
				  ) // we've found a new least
				{
					// Unselect the old least (if we have one)
					if(least)
						least->selected = false;
					least = currentSort;
					// Select the new one
					least->selected = true;
				}
			}

			currentSort = currentSort->next;
		}

		if(least) // If we found an unselected node
		{
			// Add the just-found next-least node to the new, sorted list
			// (note that in the process, we're destroying the original non-sorted list - that's
			// OK, since we have pointers to all nodes saved in the Process_sort list and are thus
			// going to reconstruct the full list in the sorting process)
			current->next = least->proc;
			current->next->next = 0;
			current = current->next;
		}
		else // all nodes are selected, so sorting is done
			break;
	}
	// Point root to the head of the new sorted list
	root = sortedListHead.next;

	// Deallocate the Process_sort list (no longer needed since sorting is complete)
	for(struct Process_sort *p = sortHead; p->next != 0; )
	{
		struct Process_sort *toDelete = p->next;
		p->next = p->next->next;
		free(toDelete);
	}
	free(sortHead); sortHead = 0;

	// The processes are now sorted; proceed with scheduling.

	// Make a copy of the process list for use within this function
	// (we'll be removing nodes from it, and we don't want to mess with the original...well, OK, we messed with it by sorting it, but that's OK since
	// the primary concern here is leaving it intact enough that main() isn't left with an invalid pointer)
	// The copy will have one more node than the original, namely, the root is a dummy - this will work better with how I've structured my scheduling loop.
	struct Process *copyRoot = malloc(sizeof(struct Process));
	copyRoot->next = 0;
	{ // Yes, the anonymous block is intentional - I wanted to put both of the next lines in the for() initializer, but it didn't like the comma operator there,
		// so I'm using the block instead to make sure p and q go out of scope (for clarity since I'm using different p and q loop pointers later).
		struct Process *p = root;
		struct Process *q = copyRoot;
		for(; p != 0; p = p->next, q = q->next)
		{
			struct Process *newNode = malloc(sizeof(struct Process));

			newNode->id = p->id;
			newNode->arrive = p->arrive;
			newNode->duration = p->duration;
			newNode->priority = p->priority;

			newNode->next = 0;
			q->next = newNode;
		}
	}

	// Set up list of slots
	// (root is a dummy node, to work better with how I've structured my loop; it'll be removed at the end for consistency with the rest of the program)
	struct cpuSlot *rootSlot = malloc(sizeof(struct cpuSlot));
	rootSlot->next = 0;
	struct cpuSlot *pSlot = rootSlot;

	// Each loop iteration schedules one process and runs it to completion (since this is nonpreemptive)
	int startTime = 0;
	while(copyRoot->next) // copyRoot->next will be 0 (all nodes removed) when there are no more processes to schedule
	{
		// Find the first process in the list that's arrived at or before the current time (startTime).
		// Since it's already sorted accordingly, this will be the one we want to schedule next.
		// Once we've found it, we'll remove it from the list so we don't encounter it again.
		bool scheduledAProcess = false;
		{ // anonymous block is intentional, as above
			struct Process *p = copyRoot->next;
			struct Process *prev = copyRoot;
			for(; p != 0; prev = p, p = p->next)
			{
				if(p->arrive <= startTime)
				{
					// Schedule this process - construct a cpuSlot node and add it to the slots list
					struct cpuSlot *newSlot = malloc(sizeof(struct cpuSlot));
					newSlot->next = 0;

					newSlot->procId = p->id;
					newSlot->startTime = startTime;
					newSlot->duration = p->duration;
					newSlot->wait = startTime - p->arrive;
					newSlot->end = startTime + p->duration;
					newSlot->turnaround = newSlot->wait + p->duration;

					startTime += p->duration;

					// Add the new slot to the slots list
					pSlot->next = newSlot;
					pSlot = pSlot->next;

					// Remove the process from the list of unscheduled processes
					prev->next = p->next;
					free(p);
					p = prev;

					scheduledAProcess = true;
					break;
				}
			}
		}

		if(!scheduledAProcess) // there were no remaining processes that have arrived yet, but there might be more down the line
		{
			// Advance one second (idle)
			startTime++;
			(*downTime)++;
		}
	}

	// Deallocate the copy we made of the process list
	for(struct Process *p = copyRoot; p != 0; )
	{
		struct Process *toDelete = p;
		p = p->next;
		free(toDelete);
	}
	copyRoot = 0;

	// Delete the dummy node from the head of the slot list, and return the dummy-less list
	pSlot = rootSlot->next;
	free(rootSlot);
	return pSlot;
}
struct cpuSlot* priority(struct Process* root, int quantum, int* downTime) {

	// Sort the list of processes by priority first, arrival time second, and PID third (all lowest-first)
	// (note: they're already sorted by PID, so we don't need to sort explicitly on that)

	// Create a Process_sort linked list to keep track of the processs during sorting
	struct Process *current = root;
	struct Process_sort *sortHead = malloc(sizeof(struct Process_sort));
	sortHead->proc = root;
	sortHead->selected = false;
	sortHead->next = 0;
	struct Process_sort *currentSort = sortHead;
	while(current->next != 0)
	{
		current = current->next;

		currentSort->next = malloc(sizeof(struct Process_sort));
		currentSort = currentSort->next;

		currentSort->proc = current;
		currentSort->selected = false;
		currentSort->next = 0;
	}

	// Sort the processes using an insertion sort
	struct Process sortedListHead; // dummy node at the head of the new sorted list to make things easier
	current = &sortedListHead;
	while(true)
	{
		// Traverse the list to find the least node that we haven't yet selected (put into place in the sorted list)
		struct Process_sort *least = 0;
		
		currentSort = sortHead;
		while(currentSort != 0)
		{
			if(!currentSort->selected) // we haven't yet put this element into place in the sorted list
			{
				if(least == 0 || 
					(currentSort->proc->priority < least->proc->priority) ||
					(currentSort->proc->priority == least->proc->priority && currentSort->proc->arrive < least->proc->arrive)
				  ) // we've found a new least
				{
					// Unselect the old least (if we have one)
					if(least)
						least->selected = false;
					least = currentSort;
					// Select the new one
					least->selected = true;
				}
			}

			currentSort = currentSort->next;
		}

		if(least) // If we found an unselected node
		{
			// Add the just-found next-least node to the new, sorted list
			// (note that in the process, we're destroying the original non-sorted list - that's
			// OK, since we have pointers to all nodes saved in the Process_sort list and are thus
			// going to reconstruct the full list in the sorting process)
			current->next = least->proc;
			current->next->next = 0;
			current = current->next;
		}
		else // all nodes are selected, so sorting is done
			break;
	}
	// Point root to the head of the new sorted list
	root = sortedListHead.next;

	// Deallocate the Process_sort list (no longer needed since sorting is complete)
	for(struct Process_sort *p = sortHead; p->next != 0; )
	{
		struct Process_sort *toDelete = p->next;
		p->next = p->next->next;
		free(toDelete);
	}
	free(sortHead); sortHead = 0;

	// The processes are now sorted; proceed with scheduling.

	// Make a copy of the process list for use within this function
	// (we'll be removing nodes from it, and we don't want to mess with the original...well, OK, we messed with it by sorting it, but that's OK since
	// the primary concern here is leaving it intact enough that main() isn't left with an invalid pointer)
	// The copy will have one more node than the original, namely, the root is a dummy - this will work better with how I've structured my scheduling loop.
	struct Process *copyRoot = malloc(sizeof(struct Process));
	copyRoot->next = 0;
	{ // Yes, the anonymous block is intentional - I wanted to put both of the next lines in the for() initializer, but it didn't like the comma operator there,
		// so I'm using the block instead to make sure p and q go out of scope (for clarity since I'm using different p and q loop pointers later).
		struct Process *p = root;
		struct Process *q = copyRoot;
		for(; p != 0; p = p->next, q = q->next)
		{
			struct Process *newNode = malloc(sizeof(struct Process));

			newNode->id = p->id;
			newNode->arrive = p->arrive;
			newNode->duration = p->duration;
			newNode->priority = p->priority;

			newNode->next = 0;
			q->next = newNode;
		}
	}

	// Set up list of slots
	// (root is a dummy node, to work better with how I've structured my loop; it'll be removed at the end for consistency with the rest of the program)
	struct cpuSlot *rootSlot = malloc(sizeof(struct cpuSlot));
	rootSlot->next = 0;
	struct cpuSlot *pSlot = rootSlot;

	// Each loop iteration schedules one process and runs it for one quantum, or until completion if it can finish in less than a quantum
	int startTime = 0;
	while(copyRoot->next) // copyRoot->next will be 0 (all nodes removed) when there are no more processes to schedule
	{
		// Find the first process in the list that's arrived at or before the current time (startTime).
		// Since it's already sorted accordingly, this will be the one we want to schedule next.
		// Once we've found it, we'll remove it from the list so we don't encounter it again.
		bool scheduledAProcess = false;
		{ // anonymous block is intentional, as above
			struct Process *p = copyRoot->next;
			struct Process *prev = copyRoot;
			for(; p != 0; prev = p, p = p->next)
			{
				if(p->arrive <= startTime)
				{
					// Schedule this process - construct a cpuSlot node and add it to the slots list
					struct cpuSlot *newSlot = malloc(sizeof(struct cpuSlot));
					newSlot->next = 0;

					newSlot->procId = p->id;
					newSlot->startTime = startTime;
					if(p->duration > quantum)
						newSlot->duration = quantum;
					else
						newSlot->duration = p->duration;
					newSlot->wait = 0; // wait is meaningless for preemptive slots; we'll need to calculate it per-process at the end
					newSlot->end = startTime + newSlot->duration;
					newSlot->turnaround = 0; // as with wait, this will need to be calculated on a per-process basis at the end

					p->duration -= newSlot->duration; // since we have our own copy of the process list, we can use duration to keep track of remaining time

					startTime += newSlot->duration;

					// Add the new slot to the slots list
					pSlot->next = newSlot;
					pSlot = pSlot->next;

					// If the process has finished, remove it from the list of unscheduled processes
					if(p->duration == 0)
					{
						prev->next = p->next;
						free(p);
						p = prev;
					}

					scheduledAProcess = true;
					break;
				}
			}
		}

		if(!scheduledAProcess) // there were no remaining processes that have arrived yet, but there might be more down the line
		{
			// Advance one second (idle)
			startTime++;
			(*downTime)++;
		}
	}

	// Deallocate the copy we made of the process list
	for(struct Process *p = copyRoot; p != 0; )
	{
		struct Process *toDelete = p;
		p = p->next;
		free(toDelete);
	}
	copyRoot = 0;

	// Delete the dummy node from the head of the slot list, and return the dummy-less list
	pSlot = rootSlot->next;
	free(rootSlot);
	return pSlot;
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
	int downTime = 0;
	
	if (strcmp(policy, "fcfs") == 0) {
		struct cpuSlot* slots = fcfs(rootProcess, quantum, &downTime);
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
		rr(rootProcess, quantum);
	} else if (strcmp(policy, "priority_non") == 0) {
		struct cpuSlot* slots = priority_non(rootProcess, quantum, &downTime);
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
		struct cpuSlot* slots = priority(rootProcess, quantum, &downTime);
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

	// TODO: traverse the process and CPU-slot lists and free memory
	
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
