/*
 * IODevice.c
 *
 *  Created on: May 30, 2013
 *      Author: Toshiba Home
 */

#include <conio.h>
#include <pthread.h>
#include "process.h"
#include "pcb.h"
#include "Queue.h"
#include "cpu.h"
#include "IODevice.h"

//constructs the IODev as a thread
IODevPtr IOConstructor(int IOidnum, cpuPtr the_cpu) {
	IODevPtr devptr = (IODevPtr) malloc(sizeof(IODev));

	devptr->cpu = the_cpu;
	devptr->IOID = IOidnum;
	devptr->IOAvailable = 0;
	devptr->owner = NULL;
	devptr->switchComplete = PTHREAD_COND_INITIALIZER;
	devptr->mutex = PTHREAD_MUTEX_INITIALIZER;

	pthread_create(&devptr->IO_thread, NULL, RunIOProcess, (void *) devptr);
	return devptr;
}

//destroys the IODevice.
void IODestructor(IODevPtr this) {
	pthread_cancel(this->IO_thread);
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
			if (devptr->owner != NULL) {
				srand(time(NULL));
				sleep((rand() % 5) + 1);
				if (devptr->IOID == 1) {
					InterruptHandler(3, devptr->owner);
				} else {
					InterruptHandler(4, devptr->owner);
				}
				pthread_cond_wait(&devptr->switchComplete, &devptr->mutex);
			} else {
				sleep(1);
		}
	}
}
//prints out the IO ID, the owner, and the waiting queue
void printIO(IODevPtr theIO) {
	if (theIO->owner->pid != -1) {
		printf("I/O%d - P%d", theIO->IOID, theIO->owner->pid);
		printQueue(theIO->IOQ);
	}
}
