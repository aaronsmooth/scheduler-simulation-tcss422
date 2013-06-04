/*
 *	cpu.h
 *
 *	Created on: 5/29/2013
 *	Author:	Aaron Nelson, Ankita Tiku, Michael Carr
 */

#ifndef CPU_H_
#define CPU_H_

 #include "process.h"
 #include "global.h"
 #include "mutex.h"
 #include "Keyboard.h"

typedef struct {

	int count; //total time count, between 1000 and 10000
	int runCount; //current run count
	int isRunning; //interrupt status, 1 = running, 0 = not running
	//int timerINT; //0 = no interrupt, 1 = interrupt
	//int IO1Comp; //0 = no interrupt, 1 = interrupt
	//int IO2Comp; //0 = no interrupt, 1 = interrupt
	//int KbComp; //0 = no interrupt, 1 = interrupt
	//int KbReq; //0 = no request trap, 1 = request trap
	int schedulerReady;	//0 = ready, 1 = not ready
	int memoryLocations; //Memory location for P|C data
	PCBPtr runningPCB; //the currently running PCB in the CPU
	QueuePtr schedulerQueue;
	QueuePtr ReadyQueue, IoQueue; //Queues for respective structures


	//mutexPtr Mutex; //Array of Mutex structs that hold information about each mutex lock
	KBDevPtr Keyboard; //Keyboard structure
	IODevPtr IO1; //IO Device 1 structure
	IODevPtr IO2; //IO Device 2 structure

	//pthread_mutex_t memoryMutexes[MUTEXARRSIZE]; //an array of mutexes for the p|c memory accessing.
	pthread_t cpu_thread; //cpu thread
	pthread_t IO1_thread; //IO1 thread
	pthread_t IO2_thread; //IO2 thread
	pthread_t Keyboard_thread; //keyboard thread
	pthread_t producer_thread; //producer
	pthread_t consumer_thread; //consumer
	//pthread_cond_t timer; //???????
	//pthread_cond_t keyboarddone; //???????
	//pthread_cond_t switchcomplete; //???????
	//pthread_mutex_t mutex; //???????


} cpuObj;

typedef cpuObj *cpuPtr;

cpuPtr cpuConstructor();

void *CPURun(void *);

void *incCount(void *);

void *resetCount(void *);

void InterruptHandler(int, PCBPtr);

#endif /* CPU_H_ */
