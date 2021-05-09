#ifndef CIRCULAR_QUEUE_H
#define CIRCULAR_QUEUE_H
//struct definition for circular queue
//implementing a queue datastructure onto the contiguous
//block of memory names
typedef struct queue {
	//elements is a pointer to a pointer to a char
	char **names;
	int currSize;
	int front;
	int rear;
	int size;
}queue;

//functions needed to implement queue
int createQueue(queue *q, int size);
int enqueue(queue *q, char *name);
char* dequeue(queue *q);
//functions for checking status of queue
int isEmpty(queue *q);
int isFull(queue *q);
//function that frees memory allocated for the queue
void deleteQueue(queue *q);

#endif