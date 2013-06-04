/*
 * main.c
 *
 *  Created on: May 21, 2013
 *      Author: Aaron Nelson
 */
#include <stdio.h>
#include <stdlib.h>
#include "process.h"
#include "global.h"
#include "cpu.h"
//#include <conio.h>

int main(int argc, char *argv[]) {
	//printf("working");

	if (argc == 9) {
		process_count = atoi(argv[2]);
		kproc_count = atoi(argv[4]);
		ioproc_count = atoi(argv[6]);
		pcproc_count = atoi(argv[8]);
		calcproc_count = process_count - (kproc_count + ioproc_count + (pcproc_count * 2));
	} else {
		process_count = 15;
		kproc_count = 3;
		ioproc_count = 3;
		pcproc_count = 3;
		calcproc_count = 3;
	}

	ReadyQPtr = QueueConstructor();

	int i;
	for (i = 0; i < kproc_count; i++) {
		PCBPtr a = PCBConstructor(processidcount, 2, 2, -1);
		enqueue(ReadyQPtr, a);
		processidcount++;
	}
	for (i = 0; i < pcproc_count; i++) {
		PCBPtr c = PCBConstructor(processidcount, 3, 2, i);
		enqueue(ReadyQPtr, c);
		processidcount++;
		PCBPtr d = PCBConstructor(processidcount, 4, 2, i);
		enqueue(ReadyQPtr, d);
		processidcount++;
	}
	for (i = 0; i < calcproc_count; i++) {
		PCBPtr e = PCBConstructor(processidcount, 0, 2, -1);
		enqueue(ReadyQPtr, e);
		processidcount++;
	}
	for (i = 0; i < ioproc_count; i++) {
		PCBPtr b = PCBConstructor(processidcount, 1, 2, -1);
		printf("\nfirst io list value: %d", b->process->requests[0]);
		enqueue(ReadyQPtr, b);
		processidcount++;
	}
	for (i = 0; i < pcproc_count; i++) {
		sharedMemory[i] = 0;
	}
	//initialize interrupt flags
	IO1INT = 0;
	IO2INT = 0;
	KBINT = 0;
	TIMERINT = 0;
	//printf("\nProcess Count %d", process_count);
	printQueue(ReadyQPtr);
	//printf("\nProcess Count2");
	cpuPtr cpu = cpuConstructor();
	printf("\nCPU Program Complete");

	//CPURun(cpu);

	return 0;
}
