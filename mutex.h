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
		QueuePtr mutexQueue;
} mutexObj;

typedef mutexObj *mutexPtr;

mutexPtr mutexConstructor(int);

void mutexDestructor(mutexPtr);

#endif /* MUTEX_H_ */
