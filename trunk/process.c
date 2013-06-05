/*
 * process.c
 *
 *  Created on: May 25, 2013
 *      Author: Aaron Nelson
 */

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include "global.h"
#include "process.h"

//constructs a process.
ProcessPtr ProcessConstructor(int type, int numofio) {

	ProcessPtr p = (ProcessPtr) malloc(sizeof(ProcessStr));

	p->proc_type = type;

	srand(time(NULL));

	p->no_steps = (rand() % 50) + 1;
	p->no_requests = numofio;
	int requestslist[50];
	int i;
	for (i = 0; i < 50; i++) { //request list is full of -1's if there are no actual requests that need to be made.
		if (type == 1 && numofio > 0) {
			int j;
			for(j = 0; j < numofio; j++) { //creates an io request, only creates as many as there are io devices.
				requestslist[i] = (rand() % 12);
			}
		} else {
			int k;
			for(k = 0;k < numofio; k++) {
				requestslist[i] = -1;
			}
		}
	}
	p->requests = requestslist;

	return p;
}

PCBPtr PCBConstructor(int id, int type, int IOCount, int sharedMem) {
	PCBPtr apcb = (PCBPtr) malloc(sizeof(PCBStr));
	srand(time(NULL));

	apcb->pid = id;
	apcb->count = rand() % 20;
	apcb->curr_count = apcb->count;

	apcb->process = ProcessConstructor(type, IOCount);
	if (type == 2) {
		apcb->state = 3;
	} else {
		apcb->state = 1;
	}
	apcb->waiting_on = 0;
	apcb->owns = -1;
	if ((type == 3) || (type == 4)) {
		apcb->sharedMemInd = sharedMem;
	} else {
		apcb->sharedMemInd = -1;
	}
	return apcb;
}

QueuePtr QueueConstructor() {
	QueuePtr q = (QueuePtr) malloc(sizeof(Queue));

	q->first = 0;
	q->last = 0;
	q->count = 0;
	return q;
}

IODevPtr IOConstructor(int IOidnum, QueuePtr IOQueue) {
	IODevPtr devptr = (IODevPtr) malloc(sizeof(IODev));

	devptr->IOID = IOidnum;
	devptr->IOAvailable = 0;
	devptr->owner = NULL;
	devptr->IOQ = IOQueue;
	//devptr->switchComplete = PTHREAD_COND_INITIALIZER;
	//devptr->mutex = PTHREAD_MUTEX_INITIALIZER;

	//pthread_create(&devptr->IO_thread, NULL, RunIOProcess, (void *) devptr);
	return devptr;
}

//destroys the IODevice.
void IODestructor(IODevPtr this) {
	free(this);
}

//sets the owner of the IODevice to a pcb, or enqueue's it if another pcb is the owner.
void IORequest(IODevPtr adevptr, PCBPtr apcb) {
	if (!adevptr->IOAvailable) {
		adevptr->IOAvailable = 1;
		adevptr->owner = apcb;
		apcb->owns = adevptr->IOID;
	} else {
		enqueue(adevptr->IOQ, apcb);
	}
}

//PCBPtr IOComplete();

//the function running that generates an interrupt to the cpu after a random time
//between 1 and 5 seconds. After the interrupt request, it waits for a signal from
//the cpu that the pcb was successfully moved to the ready queue.
void *RunIOProcess(void *args) {
	IODevPtr devptr = (IODevPtr) args;
	while(1) {
		sleep(1);
		if (devptr->owner != NULL) {
			sleep((rand() % 5) + 1);
			if (devptr->IOID == 1) {
				IO1INT = 1;
			} else {
				IO2INT = 1;
			}
		}
	}
	pthread_exit(NULL);
}
//prints out the IO ID, the owner, and the waiting queue
void printIO(IODevPtr theIO) {
	if (theIO->owner->pid != -1) {
		printf("I/O%d - P%d", theIO->IOID, theIO->owner->pid);
		printQueue(theIO->IOQ);
	}
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
	PCBPtr x = PCBConstructor(-1, -1, 0, -1);
	if (q->first != q->last) {
		x = q->q[ q->first ];
		q->first = (q->first + 1) % QUEUE_SIZE;
		q->count = q->count - 1;
	}
	return x;
}

//prints the current queue's contents. If the queue is empty, nothing prints out.
void printQueue(QueuePtr q) {
	if ((q->first != q->last)) {
		int i;
		int j = q->last;
		PCBPtr PCB = q->q[q->first];
		printf("\n");
		printf("P%d", getPCBId(PCB));
		for (i = q->first + 1; i < j; i++) {
			Queue current_queue = *q;
			PCBPtr aPCBPtr = current_queue.q[i];
			int PCBID = getPCBId(aPCBPtr);
			printf(", P%d", PCBID);
		}
		//printf("Idle");
	}
	//printf("Queue Complete");
}

//destructs a pcb
void PCBDestructor(PCBPtr pcb) {
	free(pcb);
}

//Destroys a process.
void ProcessDestructor(ProcessPtr this) {
	free(this);
}


//gets the pcb ID number.
int getPCBId(PCBPtr pcb) {
	return pcb->pid;
}
/*
int main(argc, argv) {
	if (argc == 2) {
		printf("2 argument:");
	} else {
		printf("more than 2 args");
	}
	ProcessPtr newProcPtr = ProcessConstructor(1, 2);
	ProcessStr newProcStr = *newProcPtr;
	printf("\n proc type = %d", newProcStr.proc_type);
	printf("\n proc step numbers = %d", newProcStr.no_steps);
	printf("\n proc num of req = %d", newProcStr.no_requests);
	int i;
	printf("\n proc array list [");
	//int thelist[50] = (newProcPtr->requests)->;
	//int alist = *list;
	for(i = 0; i < newProcStr.no_requests; i++) {
		printf(" %d ", (newProcStr.requests)[i]);
	}
}*/

