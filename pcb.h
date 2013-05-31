/*
 * pcb.h
 *
 *  Created on: May 22, 2013
 *      Author: Aaron Nelson
 */

/*
* pcb.h
*
* Header file for a module of process control block
*
*/


//#include "process.h"

typedef struct {
	int pid;
	int count; //Random amount of cpu cycles this pcb will run for
	int curr_count; //The pcb's current number of cpu cycles it has run for
	int state; //0 = running, 1 = ready, 2 = interrupted, 3 = blocked
	ProcessPtr process; //pointer to the actual process
	int waiting_on; //which queue it is in if it is blocked, 0 = no queue,
			//1 = ready queue, 2 = blocked queue1, 3 = blocked queue2, 4 = mutex queue,
			//5 = cond variable queue
	int owns; //which mutex locks it current holds, -1 if it holds none.

} PCBStr;

typedef PCBStr *PCBPtr;

//constructs a PCB taking an integer value for the process ID,
//another int for the process type it is and an integer value for the number of I/O
//process are in the running program from the user.
PCBPtr PCBConstructor(int, int, int);

//destructs the PCB
void PCBDestructor(PCBPtr);

int getPCBId(PCBPtr);
