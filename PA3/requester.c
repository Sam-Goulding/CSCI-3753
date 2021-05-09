#include "requester.h"

#define MAX_NAME_LENGTH 255 //max size of a hostname including null terminator

void* requester(void *helper) {
	//helper is a pointer to a threadHelper struct
	threadHelper* threadArgs = (threadHelper *)helper;
	//track how many files each thread has parsed
	int filesServiced = 0;
	//this threads PID
	int tID = pthread_self();
	//if every file already has its own thread, this thread won't do anything useful
	if(threadArgs->identifier > threadArgs->totalFiles) {
		//print to stdout (don't need to write to log)
		pthread_mutex_lock(threadArgs->stdoutMutex);
		printf("thread %d serviced %d files\n", tID, filesServiced);
		pthread_mutex_unlock(threadArgs->stdoutMutex);
		return NULL;
	}
	//thread can still be assigned its own file
	else {
		//file handle
		char *filename;
		//temp variable for storing buffer contents - pushed to shared buffer
		char *namePtr;
		//buffer for fscanf - 256 to check against max length
		char nameBuffer[256];
		//loop until the file queue is empty
		//acquire fileQueueMutex
		pthread_mutex_lock(threadArgs->fileQueueMutex);
		while(!isEmpty(threadArgs->filesLeftQueue)){
			//dequeue item
			filename = dequeue(threadArgs->filesLeftQueue);
			//release fileQueueMutex
			pthread_mutex_unlock(threadArgs->fileQueueMutex);
			//process file
			FILE * fptr = fopen(filename,"r");
			//invalid file (missing or unreadable or...)
			if(fptr == NULL) {
				pthread_mutex_lock(threadArgs->stderrMutex);
				fprintf(stderr, "Invalid File: %s\n", filename);
				pthread_mutex_unlock(threadArgs->stderrMutex);
				filesServiced++;
				//go to next file
			}
			//valid file
			else {
				//read file line by line
				while(fscanf(fptr, "%s", nameBuffer) == 1) {
					//check if line was valid length - strlen gets # bytes before '\0'
					if(strlen(nameBuffer) > MAX_NAME_LENGTH-1) {
						pthread_mutex_lock(threadArgs->stderrMutex);
						fprintf(stderr, "Hostname was too long, ignoring\n");
						pthread_mutex_unlock(threadArgs->stderrMutex);
					}
					//hostname length was valid
					else {
						//allocate memory for storing nameBuffer contents
						namePtr = malloc(strlen(nameBuffer)+1);
						//copy buffer to ptr
						strncpy(namePtr,nameBuffer,strlen(nameBuffer)+1);
						//INSERTION INTO SHARED LOG
						//acquire service mutex
						pthread_mutex_lock(threadArgs->servicedFileMutex);
						//write to serviced log file
						fprintf(threadArgs->logFile, "%s\n",namePtr);
						//release mutex
						pthread_mutex_unlock(threadArgs->servicedFileMutex);
						//INSERTION INTO SHARED BUFFER
						//acquire buffer mutex
						pthread_mutex_lock(threadArgs->sharedBufferMutex);
						//while buffer full, wait this thread until resolver signals
						while(isFull(threadArgs->sharedBufferQueue)) {
							pthread_cond_wait(threadArgs->full,threadArgs->sharedBufferMutex);
						}
						//now have a space in buffer
						enqueue(threadArgs->sharedBufferQueue, namePtr);
						//release mutex
						pthread_mutex_unlock(threadArgs->sharedBufferMutex);
						//signal resolver thread waiting
						pthread_cond_signal(threadArgs->empty);
					}
				}
				//done reading file
				fclose(fptr);
				filesServiced++;
			}
			//acquire fileQueueMutex before checking condition
			pthread_mutex_lock(threadArgs->fileQueueMutex);
		}	
		//release fileQueueMutex
		pthread_mutex_unlock(threadArgs->fileQueueMutex);
		//no more files to service from file Queue
		//output
		pthread_mutex_lock(threadArgs->stdoutMutex);
		printf("thread %d serviced %d files\n", tID, filesServiced);
		pthread_mutex_unlock(threadArgs->stdoutMutex);
		return NULL;

	}

}