/*
 * mutex.c
 *	This is a mutex object that has a queue for those waiting to acquire the mutex.
 *  Created on: May 26, 2013
 *      Author: Aaron Nelson
 */

#include <stdio.h>
#include <stdlib.h>
#include "mutex.h"

//constructs the mutex
mutexPtr mutexConstructor(int id) {
	mutexPtr q = (mutexPtr) malloc(sizeof(mutexObj));

	q->mutexID = id;
	q->owner = NULL;
	q->mutexLocked = 0;
	q->waitingPCB = NULL;

	return q;
}
//destroys the mutex
void mutexDestructor(mutexPtr this) {
	free(this);
}
//set the owner of the mutex if the mutex is currently not locked.
void setOwner(mutexPtr the_mutex, PCBPtr the_process) {
	if (!the_mutex->mutexLocked) {
		the_mutex->mutexLocked = 1;
		the_mutex->owner = the_process;
		the_process->owns = the_mutex->mutexID;
	} else {
		the_mutex->waitingPCB = the_process;
		the_process->state = 3;
	}
}
//change the owner of the mutex if the queue is not empty to the first PCB in the mutexQueue
//sets the PCB to the owner of this mutex, changes state to ready

void switchOwner(mutexPtr the_mutex) {
	if (the_mutex->waitingPCB != NULL) {
		the_mutex->owner = the_mutex->waitingPCB;
		the_mutex->owner->owns = the_mutex->mutexID;
		//the_mutex->owner->state = 1;
	} else {
		//the_mutex->mutexLocked = 0;
		the_mutex->owner = NULL;
	}
	//return the_mutex->owner->pid;
}

//Sets the current PCB's state to blocked
//Sets the current PCB's waiting on to the mutexID
//Sets the mutex's waiting PCB to the_process.
void mutexEnqueue(mutexPtr the_mutex, PCBPtr the_process) {
	the_process->state = 3;
	the_process->waiting_on = the_mutex->mutexID;
	the_mutex->waitingPCB = the_process;
}

//returns the value of the mutex lock status, 0 = unlocked, 1 = locked.
int checkLock(mutexPtr the_mutex) {
	return the_mutex->mutexLocked;
}

//changes the current mutex between locked and unlocked by whatever value is passed in. 0 = unlocked, 1 = locked.
void mutexSwitch(mutexPtr the_mutex, int the_value) {
	the_mutex->mutexLocked = the_value;
}

//prints the current mutex's information for the owner and those waiting.
void printMutex(mutexPtr the_mutex) {
	if (the_mutex->owner->pid != -1) {
		printf("\nM%d - P%d owns", the_mutex->mutexID, the_mutex->owner->pid);
		printf(", P%d is waiting", the_mutex->waitingPCB->pid);
		//int i;
		//for (i = the_mutex->mutexQueue->first; i < the_mutex->mutexQueue->last; i++) {
		//	printf(", P%d is waiting", *(the_mutex->mutexQueue)[i]);
		//}
	}
}
/*
int main(argc, argv) {

	}*/