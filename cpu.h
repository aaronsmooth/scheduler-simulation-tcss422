/*
 *	cpu.h
 *	
 *	Created on: 5/29/2013
 *	Author:	Michael Carr
 */

#ifndef CPU_H_
#define CPU_H_

typedef struct {
	int count;
	int isRunning;
	PCBPtr runningPCB;
} cpuObj;

typedef cpuObj *cpuPtr;

cpuPtr cpuConstructor();

#endif /* CPU_H_ */