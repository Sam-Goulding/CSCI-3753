/* Sam Goulding
   CSCI 3753
   4/5/2021
   PA3
*/

#include "circularQueue.h"
#include "multi-lookup.h"
#include "requester.h"
#include "resolver.h"

#include <stdio.h>


int main(int argc, char** argv) {
	//tracking total time
	struct timeval start,finish;
	gettimeofday(&start,NULL);
	int reqPool = atoi(argv[1]);
	int resPool = atoi(argv[2]);
	//ensure correct number of cmd line args
	if (argc < 5) {
		printf("Incorrect number of arguments. Format is: \n");
		printf("./multi-lookup <# req> <# res> <req log> <res log> [<input file> ...]\n");
		return 0;
	}
	//check if valid # of input files
	if(argc > 105) {
		fprintf(stderr, "Too many input files specified, max is %d\n", MAX_INPUT_FILES);
		return 0;
	}
	//check if valid # of resolver/requester threads
	if(reqPool < 1 || reqPool > 10) {
		fprintf(stderr, "Invalid # of requester threads, range is [1-10]\n");
		return 0;
	}
	if(resPool < 1 || resPool > 10) {
		fprintf(stderr, "Invalid # of resolver threads, range is [1-10]\n");
		return 0;
	}
	//check validity of res/req log files
	FILE * serviced = fopen(argv[3], "w");
	if(!serviced) {
		fprintf(stderr, "Invalid file for serviced output\n");
		return 0;
	}
	FILE * resolved = fopen(argv[4], "w");
	if(!resolved) {
		fprintf(stderr, "Invalid file for resolved output\n");
		return 0;
	}
	//command line arguments are OK

	//CREATE RELEVANT QUEUES - TO BE PASSED TO threadHelper
	//sharedBuffer queue
	queue sharedBufferQueue;
	createQueue(&sharedBufferQueue, 10);

	//input file queue
	queue filesLeftQueue;
	int totalFiles = argc-5;
	if(!createQueue(&filesLeftQueue,totalFiles)) {
		printf("error creating queue\n");
		return 0;
	}
	//insert all input files into file queue
	int fileNum = 5;
	char *filename;
	for(int i = 0; i < totalFiles; i++) {
		filename = argv[fileNum];
		//add filename t queue
		enqueue(&filesLeftQueue, filename);
		fileNum++;
	}
	//initialize an array of threadHelper objects for both res/req
	struct threadHelper reqHelper[reqPool];
	struct threadHelper resHelper[resPool];

	//declare and initialize mutexes
	pthread_mutex_t fileQueueMutex;
	pthread_mutex_init(&fileQueueMutex, NULL);

	pthread_mutex_t servicedFileMutex;
	pthread_mutex_init(&servicedFileMutex, NULL);

	pthread_mutex_t resolvedFileMutex;
	pthread_mutex_init(&resolvedFileMutex, NULL);

	pthread_mutex_t sharedBufferMutex;
	pthread_mutex_init(&sharedBufferMutex, NULL);

	pthread_mutex_t stdoutMutex;
	pthread_mutex_init(&stdoutMutex, NULL);

	pthread_mutex_t stderrMutex;
	pthread_mutex_init(&stderrMutex, NULL);

	//Initialize condition variables
	pthread_cond_t full;
	pthread_cond_init(&full, NULL);

	pthread_cond_t empty;
	pthread_cond_init(&empty, NULL);

	//array storing the pool of requester thread identifiers
	pthread_t requesterThreads[reqPool];
	//array storing the pool of resolver thread identifiers
	pthread_t resolverThreads[resPool];

	//REQUESTER THREAD CREATION:
	//for each requester identifier, populate threadHelper and call pthread_create()
	for(int i = 0; i < reqPool; i++) {
		reqHelper[i].logFile = serviced;
		reqHelper[i].identifier = i;
		reqHelper[i].totalFiles = totalFiles;
		reqHelper[i].filesLeftQueue = &filesLeftQueue;
		reqHelper[i].sharedBufferQueue = &sharedBufferQueue;
		reqHelper[i].fileQueueMutex = &fileQueueMutex;
		reqHelper[i].servicedFileMutex = &servicedFileMutex;
		reqHelper[i].sharedBufferMutex = &sharedBufferMutex;
		reqHelper[i].stdoutMutex = &stdoutMutex;
		reqHelper[i].stderrMutex = &stderrMutex;
		reqHelper[i].full = &full;
		reqHelper[i].empty = &empty;
		pthread_create(&(requesterThreads[i]), NULL, requester, &(reqHelper[i]));
	}

	//RESOLVER THREAD CREATION:
	//for each resolver identifier, populate threadHelper and...
	for(int i = 0; i < resPool; i++) {
		resHelper[i].logFile = resolved;
		resHelper[i].sharedBufferQueue = &sharedBufferQueue;
		resHelper[i].resolvedFileMutex = &resolvedFileMutex;
		resHelper[i].sharedBufferMutex = &sharedBufferMutex;
		resHelper[i].stdoutMutex = &stdoutMutex;
		resHelper[i].stderrMutex = &stderrMutex;
		resHelper[i].full = &full;
		resHelper[i].empty = &empty;
		pthread_create(&(resolverThreads[i]), NULL, resolver, &(resHelper[i]));
	}
	//join the requester threads
	for(int i = 0; i < reqPool; i++) {
		pthread_join(requesterThreads[i], NULL);
	}
	//all requester threads are now finished
	//insert a poison pill and signal - for each resolver thread
	for(int i = 0; i < resPool; i++) {
		//acquire shared buffer lock
		pthread_mutex_lock(&sharedBufferMutex);
		//while buffer full, block main thread
		while(isFull(&sharedBufferQueue)) {
			pthread_cond_wait(&full, &sharedBufferMutex);
		}
		//now have a space in buffer
		enqueue(&sharedBufferQueue, "DONE");
		//release mutex
		pthread_mutex_unlock(&sharedBufferMutex);
		//signal resolver thread waiting
		pthread_cond_signal(&empty);
	}
	//join the resolver threads
	for(int i = 0; i < resPool; i++) {
		pthread_join(resolverThreads[i], NULL);
	}

	//destroy mutexes
	pthread_mutex_destroy(&fileQueueMutex);
	pthread_mutex_destroy(&servicedFileMutex);
	pthread_mutex_destroy(&resolvedFileMutex);
	pthread_mutex_destroy(&sharedBufferMutex);
	pthread_mutex_destroy(&stdoutMutex);
	pthread_mutex_destroy(&stderrMutex);

	//destroy condition variables
	pthread_cond_destroy(&full);
	pthread_cond_destroy(&empty);

	//free memory allocated for queues
	deleteQueue(&sharedBufferQueue);
	deleteQueue(&filesLeftQueue);

	//close files (also freeing memory)
	fclose(serviced);
	fclose(resolved);
	//get time of finish
	gettimeofday(&finish, NULL);
	//print runtime
	double runtime = (finish.tv_sec - start.tv_sec) + 
	((finish.tv_usec - start.tv_usec)/1000000.0);

	printf("./multi-lookup: total time is: %f\n", runtime);
}
