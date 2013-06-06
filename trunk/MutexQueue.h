#ifndef MUTEX_QUEUE_H 
#define MUTEX_QUEUE_H
#include "mutex.h"

typedef struct MutexQueue {
	mutexPtr mq[QUEUE_SIZE]; 	//an array of mutex structure pointers
	int first, last;		//index of the first and last mutexes in the array
	int count;				//the number of mutexes in the queue
} MtxQueue;

typedef MtxQueue *MtxQueuePtr;	//a pointer to the queue.

//construct the empty queue
MtxQueuePtr MutexQueueConstructor();

//destroys the queue
void MutexQueueDestruct(MtxQueuePtr);

//add a mutex pointer to the queue
void MutexEnqueue(MtxQueuePtr, mutexPtr);

//remove a mutex pointer from the queue, returns the pointer.
mutexPtr MutexDequeue(MtxQueuePtr);

//prints only the currently waiting mutex's in the queue.
void MutexPrintQueue(MtxQueuePtr);

#endif

