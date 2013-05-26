/*
 * pcb.c
 *
 *  Created on: May 23, 2013
 *      Author: Toshiba Home
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
//#include "pcb.h"
#include "process.h"
#include "pcb.h"


PCBPtr PCBConstructor(int id, int type, int IOCount) {
	PCBPtr apcb = (PCBPtr) malloc(sizeof(PCBStr));
	srand(time(NULL));

	apcb->pid = id;
	apcb->count = rand() % 20;
	apcb->curr_count = apcb->count;
	//apcb->srvc_calls = createServiceCalls();
	apcb->state = 1;
	apcb->process = ProcessConstructor(type, IOCount);
	apcb->waiting_on = -1;
	apcb->owns = -1;

	return apcb;
}
/*
int createServiceCalls() {
	srand(time(NULL));
	int list[10];
	int j;
	for (j = 0; j < 10; j++) {
		list[j] = rand() % 30;
	}
	return list;
}
*/
void PCBDestructor(PCBPtr pcb) {
	free(pcb);
}
/*
//decriments the current count, if the current count is 0, it resets to the process count.
void decrimentCount(PCBPtr PCB) {
	PCBStr thePCB = *PCB;
	int count = thePCB->curr_count;
	if (count > 0) {
		thePCB->curr_count = count - 1;
	} else if (count == 0) {
		thePCB->curr_count = thePCB->count;
	}
}

int getCurrentCount(PCBPtr PCB) {
	PCBStr thePCB = *PCB;
	return thePCB->curr_count;
}
*/
int getPCBId(PCBPtr pcb) {
	//PCBStr thePCB = *pcb;
	return pcb->pid;
}
