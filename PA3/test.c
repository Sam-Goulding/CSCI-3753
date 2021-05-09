#include "circularQueue.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


int main () {
	//max length is 4 including null terminator - 3 characters allowed
	char name[5] = "hel";
	printf("# bytes: %lu\n",strlen(name));
	char *ptr = malloc(strlen(name)+1);
	strncpy(ptr,name,strlen(name)+1);
	printf("String Copied: %s\n",ptr);
	free(ptr);
	// queue testQueue;
	// createQueue(&testQueue,10);
	// enqueue(&testQueue, "one");
	// enqueue(&testQueue, "two");
	// enqueue(&testQueue, "three");
	// enqueue(&testQueue, "four");
	// enqueue(&testQueue, "five");
	// enqueue(&testQueue, "six");
	// enqueue(&testQueue, "seven");
	// enqueue(&testQueue, "eight");
	// enqueue(&testQueue, "nine");
	// enqueue(&testQueue, "ten");
	// enqueue(&testQueue, "eleven";
	// if(isFull(&testQueue)) {
	// 	printf("Queue is full!\n");
	// }
	// else {
	// 	printf("Something wrong, queue not full :(\n");
	// }

	// char *dequeueItem;

	// while(!isEmpty(&testQueue)) {
	// 	dequeueItem = dequeue(&testQueue);
	// 	printf("Item Dequeued: %s\n", dequeueItem);
	// }
	// dequeue(&testQueue);
	// deleteQueue(&testQueue);
}