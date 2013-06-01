/*
 * Queue.c
 *  A queue that is an array of PCBPtr's and has indexes tracking the first,
 *  last, and total count of the queue.
 *  Created on: May 22, 2013
 *      Author: Aaron Nelson
 */
#include <stdio.h>
#include <stdlib.h>
#include "process.h"
#include "pcb.h"
//#include "process.h"
#include "Queue.h"
//constructs an empty queue for holding PCBPtr's
QueuePtr QueueConstructor() {
	QueuePtr q = (QueuePtr) malloc(sizeof(Queue));

	q->first = 0;
	q->last = 0;
	q->count = 0;
	return q;
}

//destroys the queue.
void QueueDestruct(QueuePtr this){
	free(this);
}

//Adds a PCB to the end of the queue and incriments the queue count.
void enqueue(QueuePtr q, PCBPtr x) {
	q->q[ q->last ] = x;
	q->last = (q->last + 1) % QUEUE_SIZE;
	q->count = q->count + 1;
}

//removes the PCBPtr at the front of the queue and returns it. If the queue is empty,
//a PCBPtr is returned whose PCBID = -1. Decriments the queue count.
PCBPtr dequeue(QueuePtr q) {
	PCBPtr x = PCBConstructor(-1, -1, 0);
	if (q->first != q->last) {
		x = q->q[ q->first ];
		q->first = (q->first + 1) % QUEUE_SIZE;
		q->count = q->count - 1;
	}
	return x;
}

//prints the current queue's contents. If the queue is empty, nothing prints out.
void printQueue(QueuePtr q) {
	if (q->first != q->last) {
		int i;
		int j = q->last;
		PCBStr PCB = q->q[q->first];
		printf("\n");
		printf("P%d", getPCBId(PCB));
		for (i = q->first + 1; i < j; i++) {
			Queue current_queue = *q;
			PCBPtr aPCBPtr = current_queue.q[i];
			int PCBID = getPCBId(aPCBPtr);
			printf(", P%d", PCBID);
		}
		printf(" waiting");
		//printf("Idle");
	}
}
/*
void main(argc, argv) {
	QueuePtr aqueue = QueueConstructor();
	PCBPtr pcb1 = PCBConstructor(1, 0, 2);
	PCBPtr pcb2 = PCBConstructor(1, 1, 2);
	PCBPtr pcb3 = PCBConstructor(2, 0, 2);
	enqueue(aqueue, pcb1);
	enqueue(aqueue, pcb2);
	enqueue(aqueue, pcb3);
	printQueue(aqueue);
}*/
