/*
 * pcb.h
 *
 *  Created on: May 22, 2013
 *      Author: Toshiba Home
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
	//int srvc_calls[10]; //a list of cpu cycle values that this pcb will make service calls at
	int state; //0 = running, 1 = ready, 2 = interrupted, 3 = blocked
	ProcessPtr process; //pointer to the actual process
	int waiting_on; //which queue it is in if it is blocked, -1 if it's not blocked.
	int owns; //which mutex locks it current holds, -1 if it holds none.

} PCBStr;

typedef PCBStr *PCBPtr;

//constructs a PCB taking an integer value for the process ID, another int or the process type it is and an integer value for the number of I/O process are in the running program from the user.
PCBPtr PCBConstructor(int, int, int);

//destructs the PCB
void PCBDestructor(PCBPtr);
/*
//sets the pcb's current number of cpu cycles it has left to run, resets if it hits zero.
void setCount(PCBPtr);

//gets the pcb's current number of cpu cycles it has left to run.
int getCurrentCount(PCBPtr);
*/
//gets the pcb's ID number that uniquely identifies it.
int getPCBId(PCBPtr);
/*
//returns the current state of the pcb
int getState(PCBPtr);

//sets the current state of the pcb
void setState(int);

//returns the integer value that represents what queue this pcb is waiting in if it's currently blocked.
int getWaitingOn(PCBPtr);
*/

