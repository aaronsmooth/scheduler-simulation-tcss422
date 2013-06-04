/*
 *	cpu.c
 *
 *	Created on: 5/29/2013
 *	Author:	Michael Carr
 */

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include "cpu.h"

cpuPtr aCPU;
int runForCount;

cpuPtr cpuConstructor() {
	//create CPU with random count between 1000 and 10000
	cpuPtr q = (cpuPtr) malloc(sizeof(cpuObj));

	//q->timerINT = 0;
	q->schedulerReady = 0;
	q->schedulerQueue = QueueConstructor();
	q->isRunning = FALSE;
	//q->count = random_in_range( 1000, 10000 );
	q->memoryLocations = *sharedMemory;
	q->IoQueue = QueueConstructor();
	q->IO1 = IOConstructor(1);
	q->IO2 = IOConstructor(2);
	q->Keyboard = KBDevConstructor();
	//q->KeyboardQueue = QueueConstructor();
	q->runCount = 0;
	q->runningPCB = NULL;
	//create the ready queue
	q->ReadyQueue = ReadyQPtr;
	//q->switchcomplete = PTHREAD_COND_INITIALIZER;
	//q->timer = PTHREAD_COND_INITIALIZER;
	//q->keyboarddone = PTHREAD_COND_INITIALIZER;
	//q->mutex = PTHREAD_MUTEX_INITIALIZER;
	q->count = 1000;
	aCPU = q;

	//initialize the mutex array
	int m;
	for (m = 0; m < MUTEXARRSIZE; m++) {
		mutex[m] = PTHREAD_MUTEX_INITIALIZER;
		MutexMem[m] = mutexConstructor(m);
	}

	//create 2 IO devices as separate threads
	printf("\nCreate CPU thread");
	pthread_create(&q->cpu_thread, NULL, CPURun, (void *)q);
	pthread_create(&(q->IO1_thread), NULL, RunIOProcess, (void *) q->IO1);
	pthread_create(&(q->IO2_thread), NULL, RunIOProcess, (void *) q->IO2);
	pthread_create(&(q->Keyboard_thread), NULL, KBDevRun, (void *) q->Keyboard);
   // pthread_attr_destroy(&attr);
	return q;
}

/* Would like a semi-open interval [min, max)
int random_in_range (unsigned int min, unsigned int max)
{
  int base_random = rand(); in [0, RAND_MAX]
  if (RAND_MAX == base_random) return random_in_range(min, max);
   now guaranteed to be in [0, RAND_MAX)
  int range       = max - min,
      remainder   = RAND_MAX % range,
      bucket      = RAND_MAX / range;
   There are range buckets, plus one smaller interval
     within remainder of RAND_MAX
  if (base_random < RAND_MAX - remainder) {
    return min + base_random/bucket;
  } else {
    return random_in_range (min, max);
  }
}

void *Timer(void *this) {
    while(((CPUPtr)this)->isRunning) {        ///check if program is still in a running state
        //thread sleep
        sleep(WAIT_TIME);

    }
    pthread_exit(NULL);
}*/

void *CPURun(void *args) {
	cpuPtr CPU = (cpuPtr) args;

	//put the first PCB as running

	if (CPU->runningPCB == NULL) {
		CPU->runningPCB = dequeue(CPU->ReadyQueue);
		CPU->runningPCB->state = 0;
		printf("\nP%d is Running", CPU->runningPCB->pid);
	}

	int interruptOccurred = (IO1INT || IO2INT || KBINT || TIMERINT); //1 for interrupt has occurred, 0 for interrupt did not occur

	//printf("\nCPU count = %d", CPU->count);
	while(CPU->count >0) {
		//printf("\nWhile Loop1 Starting");
		//printf("\nCPU count = %d", CPU->count);

		if(interruptOccurred == 0) {
			if(CPU->runningPCB->curr_count > WAIT_TIME) {
					runForCount = WAIT_TIME;
			} else {
					runForCount = CPU->runningPCB->curr_count;
			}
		}

		while(runForCount > 0) {
			printf("\nCPU count = %d", CPU->count);

			if(TIMERINT) {
				InterruptHandler( 7, CPU->runningPCB);
			}
			if(IO1INT) {
				InterruptHandler( 3, CPU->IO1->owner);
			}
			if(IO2INT) {
				InterruptHandler( 4, CPU->IO2->owner);
			}
			if(KBINT) {
				InterruptHandler( 5, CPU->Keyboard->owner);
			}
			if (CPU->runningPCB->process->proc_type == 0) {
				printf("\nprocess count for calc proc = %d", CPU->runningPCB->count);
				printf("\nrunning count for calc proc = %d", runForCount);
				if (aCPU->runningPCB->curr_count == 1) {
					aCPU->runningPCB->curr_count = aCPU->runningPCB->count;
					aCPU->runningPCB->state = 1;
					enqueue(aCPU->ReadyQueue, aCPU->runningPCB);
					printf("\nP%d put into ready queue", aCPU->runningPCB->pid);
					aCPU->runningPCB = dequeue(aCPU->ReadyQueue);
					aCPU->runningPCB->state = 0;
					printf("\nP%d is now running", aCPU->runningPCB->pid);
				}
			} else if(CPU->runningPCB->process->proc_type == 1) {
				printf("\nInterrupt Request made");
				int i;
				for(i = 0; i < CPU->runningPCB->process->no_requests; i++) {
					if(runForCount == CPU->runningPCB->process->requests[i]) {
						//printf("\nInterrupt Request made");
						CPU->runningPCB->state = 2;
						CPU->runningPCB->curr_count = runForCount;
						InterruptHandler( 1, CPU->runningPCB);
						//interruptOccurred = 0;
					}
				}
			} else if(CPU->runningPCB->process->proc_type == 2) {
				InterruptHandler( 2, CPU->runningPCB);
				//interruptOccurred = 0;
			} else if(CPU->runningPCB->process->proc_type == 3 || CPU->runningPCB->process->proc_type == 4) {
				int currentP = CPU->runningPCB->pid;
				InterruptHandler( 6, CPU->runningPCB);
				if (currentP == CPU->runningPCB->pid) {								//check to see if the PCB is still the same (it got the lock)
					if (CPU->runningPCB->process->proc_type == 3) {						//producer type pcb
						printf("\nhere");
						sharedMemory[CPU->runningPCB->sharedMemInd]++;					//write value to memory
					} else if (CPU->runningPCB->process->proc_type == 4) {				//consumer type pcb
						sharedMemory[CPU->runningPCB->sharedMemInd]--;					//read & reset the value in memory
					}
					pthread_mutex_unlock(&mutex[CPU->runningPCB->sharedMemInd]);		//unlock the mutex for that memory location

					if (MutexMem[CPU->runningPCB->sharedMemInd]->waitingPCB != NULL) {
						MutexMem[CPU->runningPCB->sharedMemInd]->owner = MutexMem[CPU->runningPCB->sharedMemInd]->waitingPCB;
					} else {
						MutexMem[CPU->runningPCB->sharedMemInd]->owner = NULL;
					}
					aCPU->runningPCB = dequeue(aCPU->ReadyQueue);
					aCPU->runningPCB->state = 0;
					printf("\nP%d is now running", aCPU->runningPCB->pid);
				}
			}
				//interruptOccurred = 0;
			//if a process ran it's full program count,
			//then it resets and gets put at the end of the ready queue and state = ready.
			if (aCPU->runningPCB->curr_count == 0) {
				aCPU->runningPCB->curr_count = aCPU->runningPCB->count;
				aCPU->runningPCB->state = 1;
				enqueue(aCPU->ReadyQueue, aCPU->runningPCB);
				printf("\nP%d put into ready queue", aCPU->runningPCB->pid);
				aCPU->runningPCB = dequeue(aCPU->ReadyQueue);
				aCPU->runningPCB->state = 0;
				printf("\nP%d is now running", aCPU->runningPCB->pid);
			}
			runForCount--;
			CPU->count--;
		}
	}
}

void InterruptHandler(int interrupt, PCBPtr pcbRequest)
{
	switch (interrupt)
	{
	//if i/o request
	case 1:
		printf("\nI/O Request");
		aCPU->runningPCB->state = 2;
		if(aCPU->IO1->owner == NULL) {
			printf("\nP%d moved to I/O1 device", aCPU->runningPCB->pid);
			aCPU->IO1->owner = aCPU->runningPCB;
			//IORequest(aCPU->IO1, aCPU->runningPCB);
			//Call IO1 and pass pcbRequest
		}
		else if(aCPU->IO2->IOAvailable == 0) {
			//io2Free = False;
			//Call IO2 device and pass pcbRequest
			//IORequest(aCPU->IO2, aCPU->runningPCB);
			printf("\nP%d moved to I/O2 device", aCPU->runningPCB->pid);
			aCPU->IO2->owner = aCPU->runningPCB;
		}
		else {
			enqueue(aCPU->IoQueue, aCPU->runningPCB);
		}
		aCPU->runningPCB = dequeue(aCPU->ReadyQueue);
		aCPU->runningPCB->state = 0;

	break;
	//if keyboard request
	case 2:
		printf("\nKeyboard request");
		if(aCPU->Keyboard->keyboardFree == 1) {
			aCPU->runningPCB->state = 3;
			printf("\nHere");
			printf("\nBlocked Q: ");
			printQueue(aCPU->Keyboard->Blocked);
			enqueue(aCPU->Keyboard->Blocked, aCPU->runningPCB);
			printf("\nHere");
			printf("\nP%d was moved to the blocked queue", aCPU->runningPCB->pid);
		} else {
			//enqueue(BlockedQueue, pcbRequest);
			printf("\nP%d was moved to the keyboard device", aCPU->runningPCB->pid);
			aCPU->Keyboard->owner = aCPU->runningPCB;
			aCPU->runningPCB->state = 3;
			aCPU->Keyboard->keyboardFree = 1;
		}
		aCPU->runningPCB = dequeue(aCPU->ReadyQueue);
		printf("\nP%d is now running", aCPU->runningPCB->pid);
	break;
	//if i/o 1 complete
	case 3:
		IO1INT = 0;
		printf("\nI/O1 complete");
		printf("\nP%d was moved to the ready queue", pcbRequest->pid);
		pcbRequest->state = 1;
		enqueue(aCPU->ReadyQueue, pcbRequest);

		if(aCPU->IoQueue->count != 0) {
			PCBPtr pcbPtr = dequeue(aCPU->IoQueue);
			printf("\nI/O1 now holds P%d", pcbPtr->pid);
			aCPU->IO1->owner = pcbPtr;
			aCPU->IO1->IOAvailable = 1;
			//Call IO1 device and pass pcbPtr
		} else {
			aCPU->IO1->IOAvailable = 0;
			aCPU->IO2->owner = NULL;
		}
	break;
	//if i/o 2 complete
	case 4:
		IO2INT = 0;
		printf("\nI/O2 complete");
		printf("\nP%d was moved to the ready queue", pcbRequest->pid);
		pcbRequest->state = 1;
		enqueue(aCPU->ReadyQueue, pcbRequest);

		if(aCPU->IoQueue->count != 0) {
			PCBPtr pcbPtr = dequeue(aCPU->IoQueue);
			printf("\nI/O2 now holds P%d", pcbPtr->pid);
			aCPU->IO2->owner = pcbPtr;
			aCPU->IO2->IOAvailable = 1;
			//IORequest(aCPU->IO2, pcbPtr);
			//Call IO2 device and pass pcbPtr
		} else {
			aCPU->IO2->owner = NULL;
			aCPU->IO2->IOAvailable = 0;
		}
	break;
	//if keyboard complete
	case 5:
		KBINT = 0;
		printf("\nA Key was pressed");
		//PCBPtr pcb = dequeue(BlockedQueue);
		enqueue(aCPU->ReadyQueue, pcbRequest);

		if(aCPU->Keyboard->Blocked->count != 0) {
			aCPU->Keyboard->owner = dequeue(aCPU->Keyboard->Blocked);
			aCPU->Keyboard->keyboardFree = 1;
		} else {
			aCPU->Keyboard->owner = NULL;
			aCPU->Keyboard->keyboardFree = 0;
		}
	break;
	//if  producer or consumer request for mutex
	case 6:
		printf("\nMutex request made by P%d", aCPU->runningPCB->pid);
		//if (pthread_mutex_trylock(&(mutex[pcbRequest->sharedMemInd])) == 0) {
		if (MutexMem[aCPU->runningPCB->sharedMemInd]->mutexLocked == 0) {
			printf("\nM%d is now locked by P%d", aCPU->runningPCB->sharedMemInd, aCPU->runningPCB->pid);
			setOwner(MutexMem[aCPU->runningPCB->sharedMemInd], aCPU->runningPCB);
			MutexMem[aCPU->runningPCB->sharedMemInd]->mutexLocked = 1;
			//aCPU->runningPCB = dequeue(aCPU->ReadyQueue);
			//aCPU->runningPCB->state = 0;
		} else {
			printf("\nP%d was blocked", aCPU->runningPCB->pid);
			aCPU->runningPCB->state = 3;
			((MutexMem[aCPU->runningPCB->sharedMemInd])->waitingPCB) = aCPU->runningPCB;
			aCPU->runningPCB = dequeue(aCPU->ReadyQueue);
			aCPU->runningPCB->state = 0;
			printf("\nP%d is now running", aCPU->runningPCB->pid);
		}
	break;
	//if its a timer request
	case 7:
		TIMERINT = 0;
		printf("\nTimer Interrupt Occurred");
		printf("\nP%d was moved to the Ready Queue", aCPU->runningPCB->pid);
		aCPU->runningPCB->state = 1;
		enqueue(aCPU->ReadyQueue, aCPU->runningPCB);
		aCPU->runningPCB = dequeue(aCPU->ReadyQueue);
		aCPU->runningPCB->state = 0;
		printf("\nP%d is now Running", aCPU->runningPCB->pid);
		//Pass pcb to CPU
	break;
	default:
		printf("Wrong Input");
	break;
	}
}
/*
int main(argc, argv) {

	}*/
