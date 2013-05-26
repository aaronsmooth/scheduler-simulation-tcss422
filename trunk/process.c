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
}
*/
