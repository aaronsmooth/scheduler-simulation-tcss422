/*
 * process.c

 *
 *  Created on: May 23, 2013
 *      Author: Toshiba Home
 */

#include <stdio.h>
#include <time.h>
#include <stdlib.h>
//#include "pcb.h"
#include "process.h"
//#include "pcb.h"

ProcessPtr ProcessConstructor(int type, int numreq) {


	ProcessPtr p = (ProcessPtr) malloc(sizeof(ProcessStr));

	p->proc_type = type;

	srand(time(NULL));

	p->no_steps = (rand() % 50) + 1;
	p->no_requests = numreq;
	int requests[numreq];
	int *reqPtr = requests;
	if (type == 1 && numreq > 0) {
		int i;
		for(i = 0; i < numreq; i++) {
			requests[i] = (rand() % 50) + 1;
		}
	}
	p->requests = reqPtr;
	return p;
}

void ProcessDestructor(ProcessPtr this) {
	free(this);
}
