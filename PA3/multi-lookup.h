#ifndef MULTI_LOOKUP_H
#define MULTI_LOOKUP_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/time.h>
#include <fcntl.h>

#include "circularQueue.h"
#include "util.h"

#define ARRAY_SIZE 10 //size of shared buffer
#define MAX_INPUT_FILES 100
#define MAX_REQUESTER_THREADS
#define MAX_RESOLVER_THREADS

//helper struct to be passed to req/res threasd as arg at creation
typedef struct threadHelper {
	//Serviced file requester thread will be writing to
	FILE * logFile;
	//for keeping track of # of requester threads vs files available
	int identifier;
	int totalFiles;
	//requester threads access file Queue
	queue *filesLeftQueue;
	//both requester and resolver access sharedBuffer queue
	queue *sharedBufferQueue;
	//mutexes requester uses
	pthread_mutex_t *fileQueueMutex;
	pthread_mutex_t *servicedFileMutex;
	//mutexes resolver uses
	pthread_mutex_t *resolvedFileMutex;
	//mutexes both req/res use
	pthread_mutex_t *sharedBufferMutex;
	pthread_mutex_t *stdoutMutex;
	pthread_mutex_t *stderrMutex;
	//condition variables both req/res use
	pthread_cond_t *full; //requester waits on this condition v
	pthread_cond_t *empty; //resolver waits on this condition v
}threadHelper;

#endif
