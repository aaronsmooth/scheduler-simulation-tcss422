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

mutexPtr mutexConstructor(int id) {
	mutexPtr q = (mutexPtr) malloc(sizeof(mutexObj));

	q->mutexID = id;
	q->ownerID = -1;
	q->mutexLocked = 0;
	q->mutexQueue = QueueConstructor();
	return q;
}

void mutexDestructor(mutexPtr this) {
	free(this);
}
