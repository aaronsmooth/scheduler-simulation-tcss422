/*
 *	cpu.h
 *	
 *	Created on: 5/29/2013
 *	Author:	Michael Carr
 */

#ifndef CPU_H_
#define CPU_H_

typedef struct {
	int timerINT; //0 = no interrupt, 1 = interrupt;
	int IO1Comp;
	int IO2Comp;
	int IOReq;
	int KbComp;
	int KbReq;
	int schedulerReady;	//0 = ready, 1 = not ready
	QueuePtr schedulerQueue;
	int count;
	int runCount;
	int isRunning;		//interrupt status
	PCBPtr runningPCB;
	QueuePtr ReadyQueue, KeyboardQueue, IoQueue;
	mutexPtr Mutex;
	int memoryLocations[];
	KBDevPtr Keyboard;
	IODevPtr IO1;
	IODevPtr IO2;

	pthread_t cpu_thread;
	pthread_cond_t timer;
	pthread_cond_t keyboarddone;
	pthread_cond_t switchcomplete;
	pthread_mutex_t mutex;

} cpuObj;

typedef cpuObj *cpuPtr;

cpuPtr cpuConstructor();

#endif /* CPU_H_ */
