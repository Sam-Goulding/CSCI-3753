#include "resolver.h"

#define MAX_IP_LENGTH INET6_ADDRSTRLEN

void* resolver(void *helper) {
	//helper is a pointer to a threadHelper struct
	threadHelper* threadArgs = (threadHelper *)helper;
	//storing ipstring
	char IP[INET6_ADDRSTRLEN];
	//hold dequeued items
	char *dqed;
	//track hostnames resolved
	int numResolved = 0;
	//this threads PID
	int tID = pthread_self();
	//loop over shared buffer - return if pill found
	while(1) {
		//acquire shared buffer lock
		pthread_mutex_lock(threadArgs->sharedBufferMutex);
		//while buffer is empty
		while(isEmpty(threadArgs->sharedBufferQueue)) {
			pthread_cond_wait(threadArgs->empty,threadArgs->sharedBufferMutex);
		}
		//now have something to dequeue
		dqed = dequeue(threadArgs->sharedBufferQueue);
		//release mutex
		pthread_mutex_unlock(threadArgs->sharedBufferMutex);
		//signal requester thread waiting
		pthread_cond_signal(threadArgs->full);
		//process item dequeueds
		//item dqed is pill - requester threads are done running
		if(strcmp(dqed, "DONE") == 0) {
			//no more hostnames to resolve from shared queue
			pthread_mutex_lock(threadArgs->stdoutMutex);
			printf("thread %d resolved %d hostnames\n", tID, numResolved);
			pthread_mutex_unlock(threadArgs->stdoutMutex);
			return NULL;
		}
		else {
			//hostname to resolve
			//call dnslookup
			if(dnslookup(dqed,IP,MAX_IP_LENGTH) == UTIL_FAILURE) {
				//error resolving hostname
				pthread_mutex_lock(threadArgs->stderrMutex);
				fprintf(stderr,"Error resolving IP for hostname: %s\n", dqed);
				pthread_mutex_unlock(threadArgs->stderrMutex);
				//write to log file
				//acquire resolved mutex
				pthread_mutex_lock(threadArgs->resolvedFileMutex);
				//write to resolved log file
				fprintf(threadArgs->logFile, "%s, NOT_RESOLVED\n", dqed);
				pthread_mutex_unlock(threadArgs->resolvedFileMutex);
			}
			else {
				//resolved hostname
				pthread_mutex_lock(threadArgs->resolvedFileMutex);
				//write to resolved log file
				fprintf(threadArgs->logFile, "%s, %s\n", dqed, IP);
				pthread_mutex_unlock(threadArgs->resolvedFileMutex);
				//increment number of resolved hostnames
				numResolved++;
			}
		}
		//free memory allocated for the item in the shared buffer
		free(dqed);
	}
	return NULL;
}