/*
 * mutex.c
 *
 *  Created on: May 26, 2013
 *      Author: Toshiba Home
 */

#include <stdio.h>
#include <stdlib.h>
#include "process.h"
#include "pcb.h"
#include "Queue.h"
#include "mutex.h"

//constructs the mutex
mutexPtr mutexConstructor(int id) {
	mutexPtr q = (mutexPtr) malloc(sizeof(mutexObj));

	q->mutexID = id;
	q->ownerID = -1;
	q->mutexLocked = 0;
	q->mutexQueue = QueueConstructor();
	return q;
}
//destroys the mutex
void mutexDestructor(mutexPtr this) {
	free(this);
}
//set the owner of the mutex if the mutex is currently not locked.
void setOwner(mutexPtr *the_mutex, PCBPtr the_process) {
	if (!the_mutex->mutexLocked) {
		the_mutex->ownerID = the_process->pid;
		the_process->owns = the_mutex->mutexID;
	}
}
//change the owner id of the mutex if the queue is not empty
//sets the pcb to the owner of this mutex, changes state to ready
//returns the id number of the process who now owns this mutex
int switchOwner(mutexPtr the_mutex) {
	if (!(the_mutex->mutexQueue->first == the_mutex->mutexQueue->last)) {
		the_mutex->mutexLocked = 1;
		the_mutex->ownerID = dequeue(the_mutex->mutexQueue);
		the_mutex->owner->owns = the_mutex->mutexID;
		the_mutex->owner->state = 1;
	}
	return the_mutex->ownerID;
}

void mutexEnqueue(mutexPtr the_mutex, PCBPtr the_process) {
	enqueue(the_mutex, the_process);
}

int checkLock(mutexPtr the_mutex) {
	the_mutex->mutexLocked;
}

void mutexSwitch(mutexPtr the_mutex, int the_value) {
	the_mutex->mutexLocked = the_value;
}
