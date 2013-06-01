/*
 *	cpu.h
 *	
 *	Created on: 5/29/2013
 *	Author:	Aaron Nelson, Ankita Tiku, Michael Carr
 */

#ifndef CPU_H_
#define CPU_H_

typedef struct {
	int timerINT; //0 = no interrupt, 1 = interrupt
	int IO1Comp; //0 = no interrupt, 1 = interrupt
	int IO2Comp; //0 = no interrupt, 1 = interrupt
	//int IOReq; //needed?
	int KbComp; //0 = no interrupt, 1 = interrupt
	int KbReq; //0 = no request trap, 1 = request trap
	int schedulerReady;	//0 = ready, 1 = not ready
	QueuePtr schedulerQueue;
	int count; //total time count, between 1000 and 10000
	int runCount; //current run count
	int isRunning; //interrupt status, 1 = running, 0 = not running
	PCBPtr runningPCB; //the currently running PCB in the CPU
	QueuePtr ReadyQueue, KeyboardQueue, IoQueue; //Queues for respective structures
	mutexPtr Mutex; //Array of Mutex locks
	int memoryLocations[]; //Memory location for P|C data
	KBDevPtr Keyboard; //Keyboard structure
	IODevPtr IO1; //IO Device 1 structure
	IODevPtr IO2; //IO Device 2 structure

	pthread_t cpu_thread; //cpu thread
	pthread_cond_t timer; //???????
	pthread_cond_t keyboarddone; //???????
	pthread_cond_t switchcomplete; //???????
	pthread_mutex_t mutex; //???????

} cpuObj;

typedef cpuObj *cpuPtr;

cpuPtr cpuConstructor();

#endif /* CPU_H_ */
