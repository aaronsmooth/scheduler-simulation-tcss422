/*
 *	cpu.c
 *
 *	Created on: 5/24/2013
 *	Author:	Michael Carr, Aaron Nelson, Ankita Tiku
 */

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include "cpu.h"
#include "CondVar.h"
CVPtr* condVar;
mutexPtr* mutex;
cpuPtr aCPU;		//global cpu used
int runForCount;	//global count of a process' time running
int cpuRunning;		//flag to indicate whether the cpu is still running

cpuPtr cpuConstructor()
{
	cpuPtr q = (cpuPtr) malloc(sizeof(cpuObj));
	condVar = (CVPtr*) malloc(sizeof(CVPtr) * (pcproc_count * 2));
	mutex = (mutexPtr*) malloc(sizeof(mutexPtr) * pcproc_count);
	q->schedulerReady = 0;
	q->schedulerQueue = QueueConstructor();
	q->isRunning = FALSE;
	q->memoryLocations = *sharedMemory;
	q->IoQueue = QueueConstructor();
	q->IO1 = IOConstructor(1, q->IoQueue);
	q->IO2 = IOConstructor(2, q->IoQueue);
	q->Keyboard = KBDevConstructor();
	q->runCount = 0;
	q->runningPCB = NULL;
	q->count = (rand() % 9000) + 1000;
	aCPU = q;
	cpuRunning = 1;
	
	//initialize the mutex array and condition variable array
	int m = 0;
	for (; m < pcproc_count; m++)
	{
		mutex[m] = mutexConstructor(m);
	}
	for (m = 0 ; m < (pcproc_count * 2); m++)
	{
		condVar[m] = CVConstructor();
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
		
		//PRINT OUT OF CURRENT SETTINGS
		sleep(1);
		printf("\n\n\n\n\n\n\n\n");
		printf("P%d - running", aCPU->runningPCB->pid);
		if (aCPU->IO1->owner != NULL) printf("\nI/O1 - P%d", aCPU->IO1->owner->pid);
		if (aCPU->IO2->owner != NULL) printf("\nI/O2 - P%d", aCPU->IO2->owner->pid);
		if (aCPU->IoQueue->count > 0)
		{
			printf("\nI/O Waiting - ");
			printQueue(aCPU->IoQueue);
		}
		int m;
		for(m = 0; m < pcproc_count; m++)
		{
			if (mutex[m]->owner != NULL) printf("\nM%d - P%d owns", m, mutex[m]->owner->pid);
		}
		for(m = 0; m < pcproc_count * 2; m++)
		{
			if (condVar[m] != NULL ) 
			{
				if (condVar[m]->pcbQueue->count > 0)
				{
					printf("\nCV%d - ", m);
					printQueue(condVar[m]->pcbQueue);
				}
			}
		}
		//Keyboard
		//printf();
		printf("\n-------------------------");
		
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
			if (aCPU->count == 0)
			{
			cpuRunning = 0;
			KBDevDestructor(aCPU->Keyboard);
			pthread_exit(NULL);
			}
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
		int number = rand() % 2;
		if((aCPU->IO1->owner == NULL) && (number == 1) ||
		   (aCPU->IO1->owner == NULL && aCPU->IO2->owner != NULL)) {
			printf("\nP%d moved to I/O1 device", aCPU->runningPCB->pid);
			aCPU->IO1->owner = aCPU->runningPCB;
		}
		else if((aCPU->IO2->owner == NULL) && (number == 0) ||
		   (aCPU->IO2->owner == NULL && aCPU->IO1->owner != NULL)) {
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
		printf("\nP%d's I/O interrupt complete", pcbRequest->pid);
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
			printf("\nKeyboard owner given to P%d", aCPU->Keyboard->owner->pid);
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
	//printf("\nPRODUCER THREAD CREATED=================================");
	PCBPtr currentP = (PCBPtr) args;
	sleep(1);
	if ( mutexLock(currentP) == 0 ) pthread_exit(NULL);
	if (sharedMemory[currentP->sharedMemInd] == 1)
	{
		conditionWait(currentP, mutex[currentP->sharedMemInd]);
	}
	sharedMemory[currentP->sharedMemInd] = 1;
	printf("\nShared Memory[%d] = 1 ((Changed by M%d owner - P%d))", currentP->sharedMemInd, currentP->sharedMemInd, mutex[currentP->sharedMemInd]->owner->pid);
	sleep(1);
	conditionSignal(currentP);
	mutexUnlock(currentP->sharedMemInd);
	pthread_exit(NULL);
}

//consumer thread
void *resetCount(void *args) {
	//printf("\nCONSUMER THREAD CREATED=================================");
	PCBPtr currentP = (PCBPtr) args;
	sleep(1);
	if ( mutexLock(currentP) == 0 ) pthread_exit(NULL);
	if (sharedMemory[currentP->sharedMemInd] == 0)
	{
		conditionWait(currentP, mutex[currentP->sharedMemInd]);
	}
	sharedMemory[currentP->sharedMemInd] = 0;
	printf("\nShared Memory[%d] = 0 ((Changed by M%d owner - P%d))", currentP->sharedMemInd, currentP->sharedMemInd, mutex[currentP->sharedMemInd]->owner->pid);
	sleep(1);
	conditionSignal(currentP);
	mutexUnlock(currentP->sharedMemInd);
	pthread_exit(NULL);
}

void conditionWait(PCBPtr pcb, mutexPtr mtx)
{
	int id = pcb->sharedMemInd * 2;
	if ( pcb->process->proc_type == 4 ) id++;
	if ( mutex[pcb->sharedMemInd]->owner->pid != pcb->pid )
	{
		printf("\nP%d put into CV%d", pcb->pid, pcb->sharedMemInd);
		MutexEnqueue(condVar[id]->mutexQueue, mtx);
		enqueue(condVar[id]->pcbQueue, pcb);
		condVar[id]->isWaiting = 1;
		while ( condVar[id]->isWaiting == 1 )
		{
			sleep(1);
		}
	}
	else
	{
		pthread_exit(NULL);
	}
}

void conditionSignal(PCBPtr pcb)
{
	int id = pcb->sharedMemInd * 2;
	if ( pcb->process->proc_type == 4 ) id++;
	if (condVar[id]->pcbQueue->count > 0)
	{
		condVar[id]->isWaiting = 0;
		MutexDequeue(condVar[id]->mutexQueue);
		dequeue(condVar[id]->pcbQueue);
	}
}

int mutexLock(PCBPtr pcb)
{
	if ( mutex[pcb->sharedMemInd]->owner == NULL )
	{
		mutex[pcb->sharedMemInd]->owner = pcb;
		mutex[pcb->sharedMemInd]->mutexLocked = 1;
		printf("\nM%d lock acquired by P%d", pcb->sharedMemInd, pcb->pid);
	}
	if ( mutex[pcb->sharedMemInd]->owner != NULL &&
		 mutex[pcb->sharedMemInd]->owner->pid != pcb->pid )
	{
		mutex[pcb->sharedMemInd]->waitingPCB = pcb;
		return 0;
	}
	return 1;
}

void mutexUnlock(int MtxID)
{
	if ( mutex[MtxID]->owner != NULL )
	{
		if ( mutex[MtxID]->waitingPCB != NULL )
		{
			mutex[MtxID]->owner = mutex[MtxID]->waitingPCB;
			mutex[MtxID]->waitingPCB = NULL;
			printf("\nM%d lock acquired by P%d", MtxID, mutex[MtxID]->owner->pid);
		}
		else
		{
			mutex[MtxID]->owner = NULL;
			mutex[MtxID]->mutexLocked = 0;
		}
	}
}


