/*
 * process.h
 *
 *  Created on: May 25, 2013
 *      Author: Aaron Nelson
 */

#define QUEUE_SIZE 100

#ifndef PROCESS_H_
#define PROCESS_H_

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

typedef struct {
	int pid;
	int count; //Random amount of cpu cycles this pcb will run for
	int curr_count; //The pcb's current number of cpu cycles it has run for
	int state; //0 = running, 1 = ready, 2 = interrupted, 3 = blocked
	ProcessPtr process; //pointer to the actual process
	int waiting_on; //which queue it is in if it is blocked, 0 = no queue,
			//1 = ready queue, 2 = IO1, 3 = IO2, 4 = Keyboard queue,
	int owns; //which mutex locks it current holds, -1 if it holds none.
	int sharedMemInd; 	//index to a memory location for producer/consumers to utilize.
						//-1 for all other process types

} PCBStr;

typedef PCBStr *PCBPtr;

typedef struct {
	PCBPtr q[QUEUE_SIZE]; 	//an array of PCB structure pointers
	int first, last;		//index of the first and last PCB's in the array
	int count;				//the number of PCBs in the queue
} Queue;

typedef Queue *QueuePtr;	//a pointer to the queue.


typedef struct {
	int IOID;
	int IOAvailable;	//flag variable. 0 = no current owner, 1 = another pcb owns the IO device.
	PCBPtr owner;
	QueuePtr IOQ;
} IODev;

typedef IODev *IODevPtr;

IODevPtr IOConstructor(int, QueuePtr);

void IODestructor(IODevPtr);

void IORequest(IODevPtr, PCBPtr);

//PCBPtr IOComplete();

void *RunIOProcess(void *);

void printIO(IODevPtr);
//construct the empty queue
QueuePtr QueueConstructor();

//destroys the queue
void QueueDestruct(QueuePtr);

//add a PCB pointer to the queue
void enqueue(QueuePtr, PCBPtr);

//remove a PCB pointer from the queue, returns the pointer.
PCBPtr dequeue(QueuePtr);

//prints only the currently waiting PCB's in the queue.
void printQueue(QueuePtr);

//constructs a PCB taking an integer value for the process ID,
//another int for the process type it is and an integer value for the number of I/O
//process are in the running program from the user.
//the last int is the memory index location for producer and consumers
PCBPtr PCBConstructor(int, int, int, int);

ProcessPtr ProcessConstructor(int, int); //first int is the process type, second is the number of i/o processes there will be in the program.

//destructs the PCB
void PCBDestructor(PCBPtr);
void ProcessDestructor(ProcessPtr);


int getPCBId(PCBPtr);

#endif /* PROCESS_H_ */
