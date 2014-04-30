#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <signal.h>
#include <errno.h>

#define MEM_THRESHOLD 0.75f // fraction of system memory usage we want to watch for
#define CHECK_FREQUENCY 1 // number of seconds to wait between checking system memory usage

// Kill user processes as necessary to reduce system memory usage by the specified number of KB.
// Returns the number of KB actually freed (will normally be at least as big as requested, but could be less if too much memory is tied up in system processes).
int reduceMemoryUsage(int kb);

// Check periodically to see if system memory usage exceeds a defined threshold; when it goes over that threshold, print a message to the console reflecting this.
int main()
{
	printf("Monitoring memory usage; press Ctrl-C to quit.\n");

	bool previouslyOverThreshold = false;

	// Main loop - continue running until the user presses Ctrl-C
	while(true)
	{
		//// Read from /proc/meminfo to determine system memory usage
		FILE *meminfo = fopen("/proc/meminfo", "r"); // open /proc/meminfo for reading
		size_t lineLength = 0;
		char *memTotalLine = (char*)0;
		char *memFreeLine = (char*)0;
		// Read the first two lines (MemTotal and MemFree)
		int result1 = getline(&memTotalLine, &lineLength, meminfo);
		int result2 = getline(&memFreeLine, &lineLength, meminfo);
		if(result1 == -1 || result2 == -1 || ferror(meminfo))
		{
			fprintf(stderr, "Error while reading from /proc/meminfo!\n");

			// Sleep for CHECK_FREQUENCY seconds and proceed to the next check (the error might be a transient condition, so we don't want to quit)
			sleep(CHECK_FREQUENCY);
			continue;
		}
		// Parse the total/free memory values from the lines
		int memTotal, memFree;
		result1 = result2 = -1;
		result1 = sscanf(memTotalLine, "MemTotal: %d kB", &memTotal);
		result2 = sscanf(memFreeLine, "MemFree: %d kB", &memFree);
		if(result1 != 1 || result2 != 1)
		{
			fprintf(stderr, "Error: failed to parse MemTotal and MemFree from /proc/meminfo contents!\n");
			free(memTotalLine);
			free(memFreeLine);
			fclose(meminfo);

			// Sleep for CHECK_FREQUENCY seconds and proceed to the next check (the error might be a transient condition, so we don't want to quit)
			sleep(CHECK_FREQUENCY);
			continue;
		}
		free(memTotalLine);
		free(memFreeLine);
		fclose(meminfo);

		// If system memory usage is over MEM_THRESHOLD%, and this wasn't the case on the last iteration, print a message to the console
		float memFreePct = (float)memFree / (float)memTotal;
		if(memFreePct < 1.0f - MEM_THRESHOLD) // threshold exceeded
		{
			if(!previouslyOverThreshold)
			{
				printf("Memory usage exceeded threshold (%f%%): %f\n", 100*MEM_THRESHOLD, 100*(1.0f - memFreePct));
				previouslyOverThreshold = true;

				int targetMemUsage = (int)(MEM_THRESHOLD * (float)memTotal);
				int kbFreed = reduceMemoryUsage((memTotal - memFree) - targetMemUsage);
				printf("Successfully freed %d KB of memory.\n", kbFreed);
			}
		}
		else
			previouslyOverThreshold = false;

		// Sleep for CHECK_FREQUENCY seconds before checking again
		sleep(CHECK_FREQUENCY);
	}

	return 0;
}

int reduceMemoryUsage(int kb)
{
	// Get the name of a temporary file in which to store the output of "ps" sorted by resident set size
	char tempFileName[FILENAME_MAX];
	tmpnam(tempFileName);

	// Run "ps" and sort its output by resident set size
	char psCommand[FILENAME_MAX + 60];
	strcpy(psCommand, "ps -e -o rss,user,pid,cmd --no-heading | sort -r -n > ");
	strcat(psCommand, tempFileName);
	system(psCommand);

	//// Read the "ps" output from file; it's sorted by descending resident set size, so we'll read a line, kill the associated process, and repeat until we've freed the
	//// requested # of KB.
	int kbFreed = 0;

	FILE *psOutput = fopen(tempFileName, "r");
	do
	{
		size_t lineLength = 0;
		char *line = (char*)0;

		if(getline(&line, &lineLength, psOutput) == -1 || ferror(psOutput))
		{
			fprintf(stderr, "Error reading output of 'ps' command; stopped halting processes prematurely.\n");
			break;
		}

		// Parse out the information 'ps' returned about this process
		int rss; // resident set size
		char *owner = (char*)malloc(sizeof(char) * sysconf(_SC_LOGIN_NAME_MAX));
		pid_t processID;
		char *command = (char*)malloc(sizeof(char) * sysconf(_SC_ARG_MAX));

		if(sscanf(line, "%d %s %d %s", &rss, owner, &processID, command) != 4)
		{
			fprintf(stderr, "Error parsing output of 'ps' command; stopped halting processes prematurely.\n");
			break;
		}
		free(line);

		// If the process is owned by root, or contains "ssh" in its name, don't kill it - skip to the next one
		if(strcmp(owner, "root") == 0 || strstr(command, "ssh") != 0)
			continue;

		// Kill the process, and add its resident set size to kbFreed.
		if(kill(processID, SIGTERM) == -1) // ask processes "nicely" to terminate (SIGTERM vs. SIGKILL)
			fprintf(stderr, "Error: failed to kill process %d; kill() returned error code: %d\n", processID, errno);
		else
		{
			printf("Killed process %d (%d KB)...\n", processID, rss);

			kbFreed += rss;
		}
	} while(kbFreed < kb);

	fclose(psOutput);

	// Delete the temporary file in which we stored the output of "ps"
	unlink(tempFileName);

	// Return the number of KB freed
	return kbFreed;
}
