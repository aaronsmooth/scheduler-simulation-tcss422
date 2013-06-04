/*
 * global.h
 */

#include "process.h"
#include "mutex.h"
#include "cpu.h"
#include <pthread.h>

#define TRUE 1
#define FALSE 0
#define WAIT_TIME 30	//the Quanta for our CPU, every process gets to run 30 loops in the CPu.
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
mutexPtr MutexMem[MUTEXARRSIZE]; //Array of Mutex structs that hold information about each mutex lock
pthread_mutex_t mutex[MUTEXARRSIZE];
pthread_cond_t condVar[MUTEXARRSIZE];
QueuePtr ReadyQPtr;
int sharedMemory[SHAREDMEMLOC];
