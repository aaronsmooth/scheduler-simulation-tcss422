/*
 * process.h
 *
 *  Created on: May 25, 2013
 *      Author: Aaron Nelson
 */

#ifndef PROCESS_H_
#define PROCESS_H_
//#define REQUESTLISTSIZE 50

typedef struct {
	int proc_type;       // code for process type, e.g. 0=compute, 1=i0, 2=keyboard, 3=producer, 4=consumer
	int	no_steps;		// number of time steps before resetting to 0 - number of instructions
	int no_requests;	// number of requests that will be generated during the process run
	int *requests;		// an array of requests, each request (e.g. io service) is issued at a specific
						// time step. These are the synchronous events that result in traps being issued.
						// You can get fancy and make this an array of RequestTypeStr which contains the
						// step number when issued, and a request type (e.g. 0=io, 1=sync, etc.)
} ProcessStr;

typedef ProcessStr *ProcessPtr;

ProcessPtr ProcessConstructor(int, int); //first int is the process type, second is the number of i/o processes there will be in the program.

void ProcessDestructor(ProcessPtr);

#endif /* PROCESS_H_ */
