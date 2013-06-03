/*
 * main.c
 *
 *  Created on: May 21, 2013
 *      Author: Aaron Nelson
 */
#include <stdio.h>
#include "cpu.h"
#include "process.h"
#include "global.h"
#include <conio.h>

void main(int argc, char *argv[]) {
	printf("working");
	//processidcount = 0;
	if (argc == 9) {
		process_count = atoi(argv[2]);
		kproc_count = atoi(argv[4]);
		ioproc_count = atoi(argv[6]);
		pcproc_count = (atoi(argv[8]));
		calcproc_count = process_count - (kproc_count + ioproc_count + pcproc_count);
	} else {
		process_count = 10;
		kproc_count = 2;
		ioproc_count = 2;
		pcproc_count = 2;
		calcproc_count = 4;
	}
	//mutex[pcproc_count];

	ReadyQPtr = QueueConstructor();

	int i;
	for (i = 0; i < kproc_count; i++) {
		PCBPtr a = PCBConstructor(processidcount, 2, 2, -1);
		enqueue(ReadyQPtr, a);
		processidcount++;
	}
	for (i = 0; i < ioproc_count; i++) {
		PCBPtr b = PCBConstructor(processidcount, 1, 2, -1);
		enqueue(ReadyQPtr, b);
		processidcount++;
	}
	for (i = 0; i < pcproc_count; i++) {
		PCBPtr c = PCBConstructor(processidcount, 3, 2, i);
		enqueue(ReadyQPtr, c);
		processidcount++;
		PCBPtr d = PCBConstructor(processidcount, 4, 2, i);
		enqueue(ReadyQPtr, d);
		processidcount++;
		//mutex[i] = mutexConstructor(i);
	}
	for (i = 0; i < calcproc_count; i++) {
		PCBPtr e = PCBConstructor(processidcount, 0, 2, -1);
		enqueue(ReadyQPtr, e);
		processidcount++;
	}


	for (i = 0; i < pcproc_count; i++) {
		sharedMemory[i] = 0;
	}
	//sharedMemory = *m;
	printf("\nProgram Starting");
	for(i = 0; i < pcproc_count; i++) {

		printf("\nShared Mem %d %d", i, sharedMemory[i]);
	}
	printQueue(ReadyQPtr);
	cpuPtr cpu = cpuConstructor();

	//initialize interrupt flags
	IO1INT = 0;
	IO2INT = 0;
	KBINT = 0;
	TIMERINT = 0;
	//CPURun(cpu);
	printf("\nProcess Count %d", process_count);

}
