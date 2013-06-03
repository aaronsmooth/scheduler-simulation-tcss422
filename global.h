/*
 * global.h
 */

#include "process.h"
#include "mutex.h"
#include "cpu.h"

#define TRUE 1
#define FALSE 0
#define WAIT_TIME 30
#define SHAREDMEMLOC 50
#define MUTEXARRSIZE 60
#define RUNNING 0
#define READY 1
#define INTERRUPTED 2
#define BLOCKED 3

int IO1INT;		//flag that io1 has an interrupt 0 = none, 1 = INT
int IO2INT;		//flag for io2 INT 0 = none, 1 = INT
int KBINT;		//flag for keyboard INT 0 = none, 1 = INT
int TIMERINT;	//flag for timer INT 0 = none, 1 = INT

int process_count;
int kproc_count;
int ioproc_count;
int pcproc_count;
int processidcount;
int calcproc_count;

//cpuPtr CPU;
mutexPtr mutex[MUTEXARRSIZE];
QueuePtr ReadyQPtr;
int sharedMemory[SHAREDMEMLOC];
