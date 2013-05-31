/*
 * Queue

 *
 *  Created on: May 22, 2013
 *      Author: Aaron Nelson
 */

#define QUEUE_SIZE 100

typedef struct {
	PCBPtr q[QUEUE_SIZE]; 	//an array of PCB structure pointers
	int first, last;		//index of the first and last PCB's in the array
	int count;				//the number of PCBs in the queue
} Queue;

typedef Queue *QueuePtr;	//a pointer to the queue.

//construct the empty queue
QueuePtr QueueConstructor();

//destroys the queue
void QueueDestruct(QueuePtr);

//add a PCB pointer to the queue
void enqueue(QueuePtr, PCBPtr);

//remove a PCB pointer from the queue, returns the pointer.
PCBPtr dequeue(QueuePtr);

//prints only the currently waiting PCB's in the queue.
void printQueue(QueuePtr);
