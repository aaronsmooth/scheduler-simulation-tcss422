/*
 * IODevice.h
 *
 *  Created on: May 30, 2013
 *      Author: Aaron Nelson
 */

#ifndef IODEVICE_H_
#define IODEVICE_H_
typedef struct {
	int IOID;
	int IOAvailable;	//flag variable. 0 = no current owner, 1 = another pcb owns the IO device.
	cpuPtr cpu;
	PCBPtr owner;
	QueuePtr IOQ;
	pthread_t IO_thread;
	pthread_cond_t switchComplete;  //condition variable that cpu signals to the iodevice that
									//the pcb was moved to the end of the ready queue.
	pthread_mutex_t mutex;
} IODev;

typedef IODev *IODevPtr;

IODevPtr IOConstructor(int, cpuPtr);

void IODestructor(IODevPtr);

void IORequest(IODevPtr, PCBPtr);

//PCBPtr IOComplete();

void *RunIOProcess(void *);

void printIO(IODevPtr);
#endif /* IODEVICE_H_ */
