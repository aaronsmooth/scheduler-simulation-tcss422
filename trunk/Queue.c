/*
 * Queue.c
 *
 *  Created on: May 22, 2013
 *      Author: Toshiba Home
 */
#include <stdio.h>
#include <stdlib.h>
#include "pcb.h"
#include "process.h"
#include "Queue.h"
//#include "pcb.h"
//#include "process.h"




QueuePtr QueueConstructor() {
	QueuePtr q = (QueuePtr) malloc(sizeof(Queue));

	q->first = 0;
	q->last = 0;
	q->count = 0;
	return q;
}
void QueueDestruct(QueuePtr this){
	free(this);
}

void enqueue(Queue *q, PCBStr *x) {
	*q->q[ q->last ] = *x;
	q->last = (q->last + 1) % QUEUE_SIZE;
	q->count = q->count + 1;
}

PCBPtr dequeue(Queue *q) {
	PCBPtr x = q->q[ q->first ];
	q->first = (q->first + 1) % QUEUE_SIZE;
	q->count = q->count - 1;
	return x;
}

void printQueue(QueuePtr q) {
	int i;
	int j = q->last;
	int PCBID;
	for (i = q->first; i <= j; i++) {
		Queue current_queue = *q;
		PCBPtr aPCBPtr = current_queue.q[i];
		PCBID = getPCBId(aPCBPtr);
		printf("P%d is waiting", PCBID);
	}

}
