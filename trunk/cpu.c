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
int cpuRunning;

cpuPtr cpuConstructor()
{
	//create CPU with random count between 1000 and 10000
	cpuPtr q = (cpuPtr) malloc(sizeof(cpuObj));
	q->schedulerReady = 0;
	q->schedulerQueue = QueueConstructor();
	q->isRunning = FALSE;
	q->memoryLocations = *sharedMemory;
	q->IoQueue = QueueConstructor();
	q->IO1 = IOConstructor(1, q->IoQueue);
	q->IO2 = IOConstructor(2, q->IoQueue);
	q->Keyboard = KBDevConstructor();
	//q->T1 = TimerConstructor();
	q->runCount = 0;
	q->runningPCB = NULL;
	q->count = 5000;
	aCPU = q;
	cpuRunning = 1;

	//initialize the mutex array
	int m;
	for (m = 0; m < MUTEXARRSIZE; m++)
	{
		mutex[m] = PTHREAD_MUTEX_INITIALIZER;
		condVar[m] = PTHREAD_COND_INITIALIZER;
		MutexMem[m] = mutexConstructor(m);
	}

	//create 2 IO devices as separate threads
	pthread_create(&(q->IO1_thread), NULL, RunIOProcess, (void *) q->IO1);
	pthread_create(&(q->IO2_thread), NULL, RunIOProcess, (void *) q->IO2);
	pthread_create(&(q->Keyboard_thread), NULL, KBDevRun, (void *) q->Keyboard);
	//pthread_create(&(q->Timer_thread), NULL, RunTimer, (void *)q->T1);
	return q;
}

//CPU Thread Function
void *CPURun(void *args)
{
	//put the first PCB as running
	if (aCPU->runningPCB == NULL)
	{
		aCPU->runningPCB = dequeue(ReadyQPtr);
		aCPU->runningPCB->state = 0;
	}

	//1 for interrupt has occurred, 0 for interrupt did not occur
	int interruptOccurred = (IO1INT || IO2INT || KBINT || TIMERINT);

	while(aCPU->count > 0)
	{
		printf("\n\n\nP%d - running", aCPU->runningPCB->pid);

		if (aCPU->IO1->owner != NULL) printf("\nI/O1 - P%d", aCPU->IO1->owner->pid);
		if (aCPU->IO2->owner != NULL) printf("\nI/O2 - P%d", aCPU->IO2->owner->pid);
		
		if (aCPU->IoQueue->count > 0)
		{
			printf("\nI/O Waiting - ");
			printQueue(aCPU->IoQueue);
		}
		//Mutex MutexMem[MUTEXARRSIZE]
		int m = pcproc_count;
		if (m > 0 && MutexMem[0]->owner != NULL)
		{
			int j = 0;
			while (MutexMem[j]->owner != NULL)
			{
				printf("\nM%d - P%d owns", j, MutexMem[j]->owner->pid);
				if ( MutexMem[j]->waitingPCB != NULL ) printf(", P%d waiting", MutexMem[j]->waitingPCB->pid);
				j++;
			}
		}

		//Keyboard
		//printf();
		//CV
		//printf();
		printf("\n-------------------------\n");
		//printf("\nReady Queue: ");
		//printQueue(ReadyQPtr);
		//printf("   Count: %d\n\n", aCPU->count);
		//runForCount = 5;
		if(interruptOccurred == 0)
		{
			if(aCPU->runningPCB->curr_count > WAIT_TIME)
			{
				runForCount = WAIT_TIME;
			}
		} 
		else
		{
			runForCount = aCPU->runningPCB->curr_count;
		}
		runForCount = rand() % 7;
		
		while(runForCount > 0)
		{
			/*
			printf("\nRunning Process: P%d		COUNT = %d\n", aCPU->runningPCB->pid, aCPU->count);
			printf("\nCPU Count = %d", aCPU->count);
			printf("\ninner loop, run for count: %d", runForCount);
			if (aCPU->IO1->owner != NULL) printf("\nIO1 owner: P%d", aCPU->IO1->owner->pid);
			if (aCPU->IO2->owner != NULL) printf("\nIO2 owner: P%d", aCPU->IO2->owner->pid);
			printf("\nIO Queue: ");
			printQueue(aCPU->IoQueue);
			printf("\n");
			*/
			runForCount--;
			aCPU->runningPCB->curr_count++;
			if (aCPU->runningPCB->curr_count == WAIT_TIME) TIMERINT = 1;
			interruptOccurred = (IO1INT || IO2INT || KBINT || TIMERINT);

			if (interruptOccurred)
			{
				if(TIMERINT)
				{
					InterruptHandler( 7, aCPU->runningPCB);
					runForCount = 0;
				}
				if(IO1INT) InterruptHandler( 3, aCPU->IO1->owner);
				if(IO2INT) InterruptHandler( 4, aCPU->IO2->owner);
				if(KBINT) InterruptHandler( 5, aCPU->Keyboard->owner);
			}
			int processType = aCPU->runningPCB->process->proc_type;
			if ((processType == 0) && (runForCount != 0))
			{
				if (runForCount == 1)
				{
					TIMERINT = 1;
				}
			}
			else if ((processType == 1) && (runForCount != 0))
			{
				int i;
				for(i = 0; i < aCPU->runningPCB->process->no_requests; i++)
				{
					if ( (rand() % 10) == (rand() % 10) )
					{
						aCPU->runningPCB->state = 2;
						aCPU->runningPCB->curr_count = runForCount;
						runForCount = 0;
						InterruptHandler( 1, aCPU->runningPCB);
					}
					else if ( runForCount == 1 )
					{
						TIMERINT = 1;
					}
				}
				/*
				int flag = 0;
				int i;
				for(; flag != 1;) 
				{
					if ( (rand() % 6) == 5 ) 
					{
						aCPU->runningPCB->curr_count = /*aCPU->runningPCB->curr_count -*//* runForCount;
						InterruptHandler( 1, aCPU->runningPCB);
						flag = 1;
					}
					else if ( runForCount == 1 )
					{
						TIMERINT = 1;
						flag = 1;
					}
					if ( flag == 0 ) 
					{
					runForCount--;
					aCPU->count--;
					}
				}
				runForCount = 0;*/
			}
			else if ((processType == 2) && (runForCount != 0))
			{
				InterruptHandler( 2, aCPU->runningPCB);
				runForCount = 0;
			}
			else if ((processType == 3) || (processType == 4) && (runForCount != 0))
			{
				if(processType == 3)
				{
					pthread_create(&aCPU->producer_thread[aCPU->runningPCB->sharedMemInd], NULL, incCount, (void *) aCPU->runningPCB);
				}
				else if (processType == 4)
				{
					pthread_create(&aCPU->consumer_thread[aCPU->runningPCB->sharedMemInd], NULL, resetCount, (void *) aCPU->runningPCB);
				}
				aCPU->runningPCB->state = 1;
				printf("\nP%d returned to ready queue", aCPU->runningPCB->pid);
				enqueue(ReadyQPtr, aCPU->runningPCB);
				aCPU->runningPCB = dequeue(ReadyQPtr);
				printf("\nP%d selected from ready queue", aCPU->runningPCB->pid);
				runForCount = 0;
			}
			aCPU->count--;
			sleep(1);
			if (aCPU->count == 0)
			{
			cpuRunning = 0;
			KBDevDestructor(aCPU->Keyboard);
			pthread_exit(NULL);
			}
		//printf("\nEscape Inner Loop");
		}
	}
}

void InterruptHandler(int interrupt, PCBPtr pcbRequest)
{
	switch (interrupt)
	{
	//if i/o request
	case 1:
		aCPU->runningPCB->state = 2;
		if(aCPU->IO1->owner == NULL) {
			printf("\nP%d moved to I/O1 device", aCPU->runningPCB->pid);
			aCPU->IO1->owner = aCPU->runningPCB;
		}
		else if( aCPU->IO2->owner == NULL ) {
			printf("\nP%d moved to I/O2 device", aCPU->runningPCB->pid);
			aCPU->IO2->owner = aCPU->runningPCB;
		}
		else {
			printf("\nP%d moved to IO Queue", aCPU->runningPCB->pid);
			enqueue(aCPU->IoQueue, aCPU->runningPCB);
		}
		aCPU->runningPCB = dequeue(ReadyQPtr);
		aCPU->runningPCB->state = 0;
		printf("\nP%d selected from ready queue", aCPU->runningPCB->pid);
	break;
	//if keyboard request
	case 2:
		if(aCPU->Keyboard->keyboardFree == 1) {
			aCPU->runningPCB->state = 3;
			enqueue(aCPU->Keyboard->Blocked, aCPU->runningPCB);
			KBHASPCB = 1;
			printf("\nP%d was moved to the Keyboard's blocked queue", aCPU->runningPCB->pid);
		} else {
			aCPU->Keyboard->keyboardFree = 1;
			printf("\nP%d is now the Keyboard device's owner", aCPU->runningPCB->pid);
			aCPU->Keyboard->owner = aCPU->runningPCB;
			aCPU->runningPCB->state = 3;
		}
		aCPU->runningPCB = dequeue(ReadyQPtr);
		aCPU->runningPCB->state = 0;
		printf("\nP%d selected from ready queue", aCPU->runningPCB->pid);
	break;
	//if i/o 1 complete
	case 3:
		IO1INT = 0;
		printf("\nP%d's I/O interrupt complete", pcbRequest->pid);
		printf("\nP%d moved from I/O1 to ready queue", pcbRequest->pid);
		pcbRequest->state = 1;
		enqueue(ReadyQPtr, pcbRequest);

		if(aCPU->IoQueue->count != 0 && aCPU->IoQueue->first != aCPU->IoQueue->last ) {
			PCBPtr pcbPtr = dequeue(aCPU->IoQueue);
			printf("\nP%d moved from IO Queue, now owner of I/O1", pcbPtr->pid);
			aCPU->IO1->owner = pcbPtr;
			aCPU->IO1->IOAvailable = 1;
		} else {
			aCPU->IO1->IOAvailable = 0;
			aCPU->IO1->owner = NULL;
		}
	break;
	//if i/o 2 complete
	case 4:
		IO2INT = 0;
		printf("\nP%d's I/O interrupt complete");
		printf("\nP%d moved from I/O2 to ready queue", pcbRequest->pid);
		pcbRequest->state = 1;
		enqueue(ReadyQPtr, pcbRequest);

		if(aCPU->IoQueue->count != 0 && aCPU->IoQueue->first != aCPU->IoQueue->last) {
			PCBPtr pcbPtr = dequeue(aCPU->IoQueue);
			printf("\nP%d moved from IO Queue, now owner of I/O2", pcbPtr->pid);
			aCPU->IO2->owner = pcbPtr;
			aCPU->IO2->IOAvailable = 1;
		} else {
			aCPU->IO2->owner = NULL;
			aCPU->IO2->IOAvailable = 0;
		}
	break;
	//if keyboard complete
	case 5:
		KBINT = 0;
		enqueue(ReadyQPtr, pcbRequest);
		if(aCPU->Keyboard->Blocked->count != 0) {
			aCPU->Keyboard->owner = dequeue(aCPU->Keyboard->Blocked);
			aCPU->Keyboard->keyboardFree = 1;
		} else {
			aCPU->Keyboard->owner = NULL;
			aCPU->Keyboard->keyboardFree = 0;
			KBHASPCB = 0;
		}
	break;
	//if  producer or consumer request for mutex
	case 6:
		//printf("\nMutex request made by P%d", aCPU->runningPCB->pid);

		//if (pthread_mutex_trylock(&(mutex[pcbRequest->sharedMemInd])) == 0) {
		//if (MutexMem[aCPU->runningPCB->sharedMemInd]->mutexLocked == 0) {
		//	printf("\nM%d is now locked by P%d", aCPU->runningPCB->sharedMemInd, aCPU->runningPCB->pid);
		//	setOwner(MutexMem[aCPU->runningPCB->sharedMemInd], aCPU->runningPCB);
		//	MutexMem[aCPU->runningPCB->sharedMemInd]->mutexLocked = 1;
			//aCPU->runningPCB = dequeue(aCPU->ReadyQueue);
			//aCPU->runningPCB->state = 0;
		//} else {
		//	printf("\nP%d was blocked", aCPU->runningPCB->pid);
		//	aCPU->runningPCB->state = 3;
		//	((MutexMem[aCPU->runningPCB->sharedMemInd])->waitingPCB) = aCPU->runningPCB;
		//	aCPU->runningPCB = dequeue(aCPU->ReadyQueue);
		//	aCPU->runningPCB->state = 0;
		//	printf("\nP%d is now running", aCPU->runningPCB->pid);
		//}
	break;
	//if its a timer request
	case 7:
		TIMERINT = 0;
		printf("\nTimer Interrupt Occurred");
		printf("\nP%d was moved to the Ready Queue", aCPU->runningPCB->pid);
		aCPU->runningPCB->state = 1;
		aCPU->runningPCB->curr_count = 0;
		enqueue(ReadyQPtr, aCPU->runningPCB);
		aCPU->runningPCB = dequeue(ReadyQPtr);
		aCPU->runningPCB->state = 0;
		printf("\nP%d selected from ready queue", aCPU->runningPCB->pid);
	break;
	default:
		printf("\nWrong Input");
	break;
	}
}
//producer thread
void *incCount(void *args) {
	PCBPtr currentP = (PCBPtr) args;
	while(cpuRunning) {
	//int result = pthread_mutex_trylock(&mutex[mux->owner->sharedMemInd]);
	//printf("\n%d", result);
	//if (result != 0) {
	//	printf("\np2");
	//	//mux->waitingPCB = aCPU->runningPCB;
	//	mux->waitingPCB = aCPU->runningPCB;
	//printf("\nMutex location = %d", currentP->sharedMemInd);
		pthread_mutex_lock(&mutex[currentP->sharedMemInd]);
	//}
	//printf("\np3");
	//pthread_mutex_lock(&mutex[mux->owner->sharedMemInd]);
		switchOwner(MutexMem[currentP->sharedMemInd]);
		MutexMem[currentP->sharedMemInd]->owner = currentP;
	//printf("\n**producer thread**");
		sharedMemory[currentP->sharedMemInd]++;				//critical region where memory value is changed
		pthread_cond_signal(&condVar[currentP->sharedMemInd]);
	//printf("\nproduer complete");
		MutexMem[currentP->sharedMemInd]->owner = NULL;
		pthread_mutex_unlock(&mutex[currentP->sharedMemInd]);
		}
	pthread_exit(NULL);
}
//consumer thread
void *resetCount(void *args) {
	//printf("\n**consumer thread**");
	PCBPtr currentP = (PCBPtr) args;
	while (cpuRunning) {
	//if (pthread_mutex_trylock(&mutex[mux->owner->sharedMemInd]) != 0) {
		//mux->waitingPCB = aCPU->runningPCB;
		//printf("\nc1");
	//	mux->waitingPCB = aCPU->runningPCB;
		pthread_mutex_lock(&mutex[currentP->sharedMemInd]);
	//}
		switchOwner(MutexMem[currentP->sharedMemInd]);
	//pthread_mutex_lock(&mutex[mux->owner->sharedMemInd]);
	//printf("\nc2");
		MutexMem[currentP->sharedMemInd]->owner = currentP;
		while(sharedMemory[currentP->sharedMemInd] == 0) {
			pthread_cond_wait(&condVar[currentP->sharedMemInd], &mutex[currentP->sharedMemInd]);
			sharedMemory[currentP->sharedMemInd] = 0;
		}
	//printf("\nConsumer complete");
		MutexMem[currentP->sharedMemInd]->owner = NULL;
		pthread_mutex_unlock(&mutex[currentP->sharedMemInd]);
	}
	pthread_exit(NULL);
}
