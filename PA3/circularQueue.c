#include <stdio.h>
#include <stdlib.h>

#include "circularQueue.h"

//implement queue functions

//create a new queue
int createQueue(queue *q, int size) {
	if(size <= 0) {
		fprintf(stderr, "Invalid queue size specified\n");
		return 0;
	}
	//createQueue only called if size isn't bigger than max
	else {
		//q->names has enough space for 'size' # of char pointers
		if(!(q->names = malloc(sizeof(char *)*size))) {
			fprintf(stderr, "Error in queue allocation\n");
			return 0;
		}
		q->currSize = 0;
		q->front = 0;
		q->rear = 0;
		q->size = size;
		//initialize each pointer as null terminator
		for(int i = 0; i < size; i++) {
			q->names[i] = '\0';
		}
		//successfull queue creation
		return 1;
	}
}

//insert into queue
int enqueue(queue *q, char *name) {
	//queue is full
	if(q->currSize == q->size) {
		fprintf(stderr, "Queue already full - cannot add\n");
		return 0;
	}
	//queue has space
	else {
		//adjust queue members
		q->names[q->rear] = name;
		q->rear = (q->rear + 1) % q->size;
		q->currSize++;
		return 1;
	}
}
//remove from queue
char* dequeue(queue *q) {
	if(q->currSize == 0) {
		fprintf(stderr, "Queue empty - cannot dequeue\n");
		return '\0';
	}
	//queue has names in it
	else {
		char *name_returned = q->names[q->front];
		q->names[q->front] = '\0';
		q->front = (q->front + 1) % q->size;
		q->currSize--;
		return name_returned;
	}
}
//check queue is empty
int isEmpty(queue *q) {
	if(q->currSize == 0) {
		return 1;
	}
	return 0;
}
//check queue is full
int isFull(queue *q) {
	if(q->currSize == q->size){
		return 1;
	}
	return 0;
}
//free memory allocated
void deleteQueue(queue *q) {
	//we will be malloc()'ing memory for each char pointer in the array
	for(int i = 0; q->names[i]; i++){
		//free each char pointer
		free(q->names[i]);
	}
	//now free the array itself
	free(q->names);
}