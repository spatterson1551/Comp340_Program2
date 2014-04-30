#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

// defines to help keep track of pipe ends
#define READ_END 0
#define WRITE_END 1

#define MEM_THRESHOLD 75.0f // percentage of system memory usage we want to watch for
#define CHECK_FREQUENCY 1 // number of seconds to wait between checking system memory usage

// Check periodically to see if system memory usage exceeds a defined threshold; when it goes over that threshold, print a message to the console reflecting this.
int main()
{
	bool previouslyOverThreshold = false;

	// Main loop - continue running until the user presses the "Q" key
	while(true)
	{
		// Read any user input that's accumulated since the last iteration and see if the "Q" key was pressed to terminate the program
		char inputBuffer[20];
		size_t bytesRead = -1;
		bool qPressed = false;
		do
		{
			bytesRead = fread(inputBuffer, sizeof(char), 20, stdin);
			for(int i = 0; i < bytesRead; i++)
				if(inputBuffer[i] == 'q' || inputBuffer[i] == 'Q')
					qPressed = true;
		} while(bytesRead == 20); // continue until all accumulated input has been consumed

		if(qPressed) // if Q was pressed, stop looping and end the program
			break;

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
		result1 = sscanf(memTotalLine, "MemTotal: %d kB", memTotal);
		result2 = sscanf(memFreeLine, "MemFree: %d kB", memFree);
		if(result1 != 1 || result2 != 1)
		{
			fprintf(stderr, "Error: failed to parse MemTotal and MemFree from /proc/meminfo contents!\n");
			free(memTotalLine);
			free(memFreeLine);

			// Sleep for CHECK_FREQUENCY seconds and proceed to the next check (the error might be a transient condition, so we don't want to quit)
			sleep(CHECK_FREQUENCY);
			continue;
		}
		free(memTotalLine);
		free(memFreeLine);

		// If system memory usage is over MEM_THRESHOLD%, and this wasn't the case on the last iteration, print a message to the console
		float memFreePct = (float)memFree / (float)memTotal;
		if(memFreePct < 1.0f - MEM_THRESHOLD) // threshold exceeded
		{
			if(!previouslyOverThreshold)
			{
				printf("Memory usage exceeded threshold (%f%%): %f\n", MEM_THRESHOLD, 1.0f - memFreePct);
				previouslyOverThreshold = true;
			}
		}
		else
			previouslyOverThreshold = false;

		// Sleep for CHECK_FREQUENCY seconds before checking again
		sleep(CHECK_FREQUENCY);
	}

	return 0;
}
