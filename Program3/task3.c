#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#define MEM_THRESHOLD 0.75f // fraction of system memory usage we want to watch for
#define CHECK_FREQUENCY 1 // number of seconds to wait between checking system memory usage

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
			}
		}
		else
			previouslyOverThreshold = false;

		// Sleep for CHECK_FREQUENCY seconds before checking again
		sleep(CHECK_FREQUENCY);
	}

	return 0;
}
