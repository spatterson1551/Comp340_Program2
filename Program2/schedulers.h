/*
  Authors: Ethan Johnson, Steve Patterson
  Course:  COMP 340, Operating Systems
  Date:    21 March 2014
  Description:	 Scheduling simulator functions for Program 2
  Compile with:  make
  Run with:      ./scheduler
*/

#pragma once

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

// For round-robin scheduling
struct ReadyQueueNode {
	struct Process *proc;
	struct ReadyQueueNode *next;
	int timeRemaining;
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

// Utility function to compute the total duration of all CPU slots
int totalTime(struct cpuSlot* root);

// Takes a list of CPU slots broken down per-quantum, and computes the sums on a per-process basis.
// Also requires the process list (for arrival times), and the number of processes involved in scheduling.
// Returns the sums in another cpuSlot list.
struct cpuSlot* postprocessSchedulingData(struct cpuSlot *slotList, struct Process *procList, int numProcesses);

//// Scheduling simulator functions

struct cpuSlot* fcfs(struct Process* root, int quantum, int* downTime);

struct cpuSlot* priority_non(struct Process* root, int quantum, int* downTime);

struct cpuSlot* priority(struct Process* root, int quantum, int* downTime);

struct cpuSlot* rr(struct Process* root, int quantum, int* downTime);