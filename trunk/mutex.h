/*
 * mutex.h
 *  This is the header file for a mutex object.
 *  Created on: May 26, 2013
 *      Author: Aaron Nelson
 */

#ifndef MUTEX_H_
#define MUTEX_H_

typedef struct {
		int mutexID;
		PCBPtr owner;
		int mutexLocked; //0 = unlocked, 1 = locked
		PCBPtr waitingPCB;
} mutexObj;

typedef mutexObj *mutexPtr;

mutexPtr mutexConstructor(int);

void mutexDestructor(mutexPtr);

void setOwner(mutexPtr, PCBPtr);

mutexPtr switchOwner(mutexPtr);

void mutexEnqueue(mutexPtr, PCBPtr);

int checkLock(mutexPtr);

void mutexSwitch(mutexPtr, int);

void printMutex(mutexPtr);

#endif /* MUTEX_H_ */
