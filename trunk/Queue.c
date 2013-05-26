/*
 * Queue.c
 *
 *  Created on: May 22, 2013
 *      Author: Toshiba Home
 */
#include <stdio.h>
#include <stdlib.h>
#include "process.h"
#include "pcb.h"
#include "Queue.h"

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
	q->q[ q->last ] = x;
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
	if (q->first != q->last) {
		int i;
		int j = q->last;
		int PCBID;
		printf("\n");
		for (i = q->first; i < j; i++) {
			Queue current_queue = *q;
			PCBPtr aPCBPtr = current_queue.q[i];
			PCBID = getPCBId(aPCBPtr);
			int pcbtype = aPCBPtr->process->proc_type;
			if (pcbtype == 0) {
				printf("compute");
			} else if (pcbtype == 1) {
				printf("io");
			}else if (pcbtype == 2) {
				printf("keyboard");
			}else if (pcbtype == 3) {
				printf("producer");
			}else if (pcbtype==4) {
				printf("consumer");
			}
			printf("%d is waiting, ", PCBID);
		}
		printf("Idle");
	} else {
		printf("Idle");
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
