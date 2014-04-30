/*
  Authors: Ethan Johnson, Steve Patterson
  Course:  COMP 340, Operating Systems
  Date:    21 March 2014
  Description:	 Scheduling simulator functions for Program 2
  Compile with:  make
  Run with:      ./scheduler
*/

#include "schedulers.h"

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

struct cpuSlot* postprocessSchedulingData(struct cpuSlot *slotList, struct Process *procList, int numProcesses)
{
	// Create a cpuSlot list to store the processed results - as in both of the priority scheduling functions,
	// the head node is a dummy, to simplify the main loop below, which will be removed before return.
	struct cpuSlot *resultHead = malloc(sizeof(struct cpuSlot));
	resultHead->next = 0;
	struct cpuSlot *pResult = resultHead;

	// For each process, create a cpuSlot struct which will contain its aggregate totals, and
	// traverse the slot list to compute these totals; then add it to the result list.
	for(int i = 0; i < numProcesses; i++)
	{
		struct cpuSlot *totals = malloc(sizeof(struct cpuSlot));
		totals->next = 0;

		// Note: wait and turnaround are computed at the end based on start, end, and (total) duration for the process as a whole.
		totals->procId = i;
		totals->startTime = 0;
		totals->duration = 0;
		totals->end = 0;

		for(struct cpuSlot *p = slotList; p != 0; p = p->next)
		{
			if(p->procId == i)
			{
				if(totals->duration == 0) // this is the first quantum for this process
					totals->startTime = p->startTime;
				totals->duration += p->duration;
				totals->end = p->end; // this overwrites what was set for end on previous iterations, which is correct since we want the end from the last quantum
			}
		}

		// Find the process in the process list and extract its arrival time (needed to compute wait time and turnaround)
		int arrivalTime;
		for(struct Process *p = procList; p != 0; p = p->next)
		{
			if(p->id == i)
			{
				arrivalTime = p->arrive;
				break;
			}
		}

		// Compute wait and turnaround
		totals->wait = totals->end - totals->duration - arrivalTime;
		totals->turnaround = totals->end - arrivalTime;

		// Add totals to the result list
		pResult->next = totals;
		pResult = pResult->next;
	}

	// Remove the dummy node at the head of the result list, and return the list
	pResult = resultHead->next;
	free(resultHead);
	return pResult;
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
	// (we'll take a total of the # of processes while we're at it)
	int numProcesses = 1;
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

		numProcesses++;
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
	currentSlot->turnaround = currentSlot->end - current->arrive;
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
		currentSlot->turnaround = currentSlot->end - current->arrive;
		if (current->arrive > (prevStart + prevDuration)) {
			*downTime += current->arrive - (prevStart + prevDuration);
		}
		prevStart = currentSlot->startTime;
		prevDuration = currentSlot->duration;
		currentSlot->next = 0;
	}
	
	// Postprocess the slot list to turn it into a per-process summary, sorted by PID
	struct cpuSlot *summaryList = postprocessSchedulingData(rootSlot, root, numProcesses);

	// Deallocate the unprocessed slot list, and return the processed one
	for(struct cpuSlot *p = rootSlot; p != 0; )
	{
		struct cpuSlot *toDelete = p;
		p = p->next;
		free(toDelete);
	}
	return summaryList;
}

struct cpuSlot* priority_non(struct Process* root, int quantum, int* downTime) {
	
	// Sort the list of processes by priority first, arrival time second, and PID third (all lowest-first)
	// (note: they're already sorted by PID, so we don't need to sort explicitly on that)

	// Create a Process_sort linked list to keep track of the processs during sorting
	// (we'll take a total of the # of processes while we're at it)
	int numProcesses = 1;
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

		numProcesses++;
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

	// Postprocess the slot list to turn it into a per-process summary, sorted by PID
	struct cpuSlot *summaryList = postprocessSchedulingData(rootSlot->next, root, numProcesses);

	// Deallocate the unprocessed slot list, and return the processed one
	for(struct cpuSlot *p = rootSlot; p != 0; )
	{
		struct cpuSlot *toDelete = p;
		p = p->next;
		free(toDelete);
	}
	return summaryList;
}

struct cpuSlot* priority(struct Process* root, int quantum, int* downTime) {

	// Sort the list of processes by priority first, arrival time second, and PID third (all lowest-first)
	// (note: they're already sorted by PID, so we don't need to sort explicitly on that)

	// Create a Process_sort linked list to keep track of the processs during sorting
	// (we'll take a total of the # of processes while we're at it)
	int numProcesses = 1;
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

		numProcesses++;
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
					newSlot->wait = 0; // wait doesn't really make sense for individual quantums; we'll need to calculate it per-process at the end
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

	// Postprocess the slot list to turn it into a per-process summary, sorted by PID
	struct cpuSlot *summaryList = postprocessSchedulingData(rootSlot->next, root, numProcesses);

	// Deallocate the unprocessed slot list, and return the processed one
	for(struct cpuSlot *p = rootSlot; p != 0; )
	{
		struct cpuSlot *toDelete = p;
		p = p->next;
		free(toDelete);
	}
	return summaryList;
}

struct cpuSlot* rr(struct Process* root, int quantum, int* downTime)
{
	// Sort the list of processes by arrival time (with PID as second criteria, but we don't need
	// to handle that explicitly since the input is guaranteed to be ordered by increasing PID).

	// Create a Process_sort linked list to keep track of the processs during sorting
	// (we'll take a total of the # of processes while we're at it)
	int numProcesses = 1;
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

		numProcesses++;
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

	// Set up list of slots
	// (root is a dummy node, to work better with how I've structured my loop; it'll be removed at the end for consistency with the rest of the program)
	struct cpuSlot *rootSlot = malloc(sizeof(struct cpuSlot));
	rootSlot->next = 0;
	struct cpuSlot *pSlot = rootSlot;

	// Keep a "queue" (another list) of just the processes that have arrived, i.e. are subject to scheduling.
	struct ReadyQueueNode *readyQueue = malloc(sizeof(struct ReadyQueueNode)); // dummy node
	readyQueue->proc = 0;
	readyQueue->next = 0;
	readyQueue->timeRemaining = 0;
	struct ReadyQueueNode *endReadyQueue = readyQueue; // points to last node in ready queue
	int processesInQueue = 0;

	// Each loop iteration schedules one process and runs it for one quantum, or until completion if it can finish in less than a quantum
	int startTime = 0;
	int prevStartTime = -1;
	struct ReadyQueueNode *lastScheduled = readyQueue; // start at dummy node
	while(true)
	{
		// Add to the ready queue any processes that have arrived between prevStartTime and startTime
		for(struct Process *p = root; p != 0; p = p->next)
		{
			if(p->arrive > prevStartTime && p->arrive <= startTime)
			{
				struct ReadyQueueNode *newReadyProc = malloc(sizeof(struct ReadyQueueNode));
				newReadyProc->next = 0;
				newReadyProc->proc = p;
				newReadyProc->timeRemaining = p->duration;

				endReadyQueue->next = newReadyProc;
				endReadyQueue = newReadyProc;

				processesInQueue++;
			}
		}

		prevStartTime = startTime;

		// If lastScheduled = 0, schedule the first process in the ready queue.
		// Otherwise, schedule the next process in the ready queue. If we reach the end, wrap around to the beginning.
		// (If we wrap all the way around to lastScheduled without scheduling a process, schedule it again - it's the only process in the ready queue.)
		// Finished processes will be left in the queue, but skipped because their timeRemaining is 0.
		bool scheduledAProcess = false;
		for(struct ReadyQueueNode *r = lastScheduled->next; r != lastScheduled; r = r->next)
		{
			if(r == 0) // end of queue
				r = readyQueue->next; // return to beginning (skipping dummy node)

			if(r->timeRemaining != 0) // process still has work to be done - schedule it
			{
				struct cpuSlot *newSlot = malloc(sizeof(struct cpuSlot));
				newSlot->next = 0;

				newSlot->procId = r->proc->id;
				newSlot->startTime = startTime;
				if(r->timeRemaining > quantum)
					newSlot->duration = quantum;
				else
					newSlot->duration = r->timeRemaining;
				newSlot->wait = 0; // will calculate this per-process at the end
				newSlot->end = startTime + newSlot->duration;
				newSlot->turnaround = 0; // same as wait

				r->timeRemaining -= newSlot->duration;

				startTime += newSlot->duration;

				// Add the new slot to the slots list
				pSlot->next = newSlot;
				pSlot = newSlot;

				scheduledAProcess = true;
				lastScheduled = r;
				break;
			}
		}
		// If we made it all the way around without scheduling a process, and lastScheduled still has time remaining, there's only one
		// process in the queue - schedule it. If lastScheduled is finished, the queue is empty, and we should advance one second (idle).
		if(!scheduledAProcess)
		{
			if(lastScheduled->timeRemaining > 0) // one process in queue
			{
				struct cpuSlot *newSlot = malloc(sizeof(struct cpuSlot));
				newSlot->next = 0;

				newSlot->procId = lastScheduled->proc->id;
				newSlot->startTime = startTime;
				if(lastScheduled->timeRemaining > quantum)
					newSlot->duration = quantum;
				else
					newSlot->duration = lastScheduled->timeRemaining;
				newSlot->wait = 0; // will calculate this per-process at the end
				newSlot->end = startTime + newSlot->duration;
				newSlot->turnaround = 0; // same as wait

				lastScheduled->timeRemaining -= newSlot->duration;

				// Add the new slot to the slots list
				pSlot->next = newSlot;
				pSlot = newSlot;
			}
			else // the queue is empty
			{
				// Advance one second (idle)
				startTime++;
				(*downTime)++;

				if(processesInQueue == numProcesses) // all processes have arrived, and finished - we're done
					break;
			}
		}
	}

	// Deallocate the ready queue
	for(struct ReadyQueueNode *p = readyQueue; p != 0; )
	{
		struct ReadyQueueNode *toDelete = p;
		p = p->next;
		free(toDelete);
	}

	// Postprocess the slot list to turn it into a per-process summary, sorted by PID
	struct cpuSlot *summaryList = postprocessSchedulingData(rootSlot->next, root, numProcesses);

	// Deallocate the unprocessed slot list, and return the processed one
	for(struct cpuSlot *p = rootSlot; p != 0; )
	{
		struct cpuSlot *toDelete = p;
		p = p->next;
		free(toDelete);
	}
	return summaryList;
}