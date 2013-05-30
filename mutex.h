/*
 * mutex.h
 *
 *  Created on: May 26, 2013
 *      Author: Toshiba Home
 */

#ifndef MUTEX_H_
#define MUTEX_H_

typedef struct {
		int mutexID;
		PCBPtr owner;
		int mutexLocked; //0 = unlocked, 1 = locked
		QueuePtr mutexQueue;
} mutexObj;

typedef mutexObj *mutexPtr;

mutexPtr mutexConstructor(int);

void mutexDestructor(mutexPtr);

void setOwner(mutexPtr, PCBPtr);

int switchOwner(mutexPtr);

void mutexEnqueue(mutexPtr, PCBPtr);

int checkLock(mutexPtr);

void mutexSwitch(mutexPtr, int);

#endif /* MUTEX_H_ */
