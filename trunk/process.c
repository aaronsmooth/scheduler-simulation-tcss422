/*
 * process.c
 *
 *  Created on: May 25, 2013
 *      Author: Toshiba Home
 */

#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include "process.h"

ProcessPtr ProcessConstructor(int type, int numofio) {

	ProcessPtr p = (ProcessPtr) malloc(sizeof(ProcessStr));

	p->proc_type = type;

	srand(time(NULL));

	p->no_steps = (rand() % 50) + 1;
	p->no_requests = numofio;
	int requestslist[50];
	int i;
	for (i = 0; i < 50; i++) {
		if (type == 1 && numofio > 0) {
			int j;
			for(j = 0; j < numofio; j++) {
				requestslist[i] = (rand() % 50) + 1;
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

void ProcessDestructor(ProcessPtr this) {
	free(this);
}
