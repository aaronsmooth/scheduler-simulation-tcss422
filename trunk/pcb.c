/*
 * pcb.c
 *
 *  Created on: May 23, 2013
 *      Author: Toshiba Home
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "process.h"
#include "pcb.h"


PCBPtr PCBConstructor(int id, int type, int IOCount) {
	PCBPtr apcb = (PCBPtr) malloc(sizeof(PCBStr));
	srand(time(NULL));

	apcb->pid = id;
	apcb->count = rand() % 20;
	apcb->curr_count = apcb->count;

	apcb->process = ProcessConstructor(type, IOCount);
	if (type == 2) {
		apcb->state = 3;
	} else {
		apcb->state = 1;
	}
	apcb->waiting_on = 0;
	apcb->owns = -1;
	return apcb;
}

void PCBDestructor(PCBPtr pcb) {
	free(pcb);
}

int getPCBId(PCBPtr pcb) {
	return pcb->pid;
}
/*
void main(argc, argv) {
	PCBPtr aptr = PCBConstructor(1, 1, 2);
	int pcbtype = aptr->process->proc_type;
	printf("\nproc id: ");
	if (pcbtype == 0) {
		printf("compute");
	} else if (pcbtype == 1) {
		printf("io");
	}else if (pcbtype == 2) {
		printf("keyboard");
	}else if (pcbtype == 3) {
		printf("producer");
	}else if (pcbtype==4) {
		printf("consumer");
	}
	printf("%d", aptr->pid);
	printf("\ncount number: %d", aptr->count);
	printf("\ncurrent count: %d", aptr->curr_count);
	printf("\nstate: %d", aptr->state);
	printf("\nwaiting on: %d", aptr->waiting_on);
	printf("\nowns: %d", aptr->owns);
}*/
