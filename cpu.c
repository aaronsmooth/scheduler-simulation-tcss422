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
	q->KeyboardQueue = QueueConstructor();
	q->Mutex = *mutex;
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
	//create the 
	

	pthread_create(&q->cpu_thread, NULL, CPURun, (void *)q);
   // pthread_attr_destroy(&attr);
	return q;
}

/* Would like a semi-open interval [min, max) */
/*int random_in_range (unsigned int min, unsigned int max)
{
  int base_random = rand(); /* in [0, RAND_MAX]
  if (RAND_MAX == base_random) return random_in_range(min, max);
  /* now guaranteed to be in [0, RAND_MAX)
  int range       = max - min,
      remainder   = RAND_MAX % range,
      bucket      = RAND_MAX / range;
  /* There are range buckets, plus one smaller interval
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

void CPURun(cpuPtr CPU) {
	//create 2 IO devices as separate threads
	pthread_create(&(CPU->IO1_thread), NULL, RunIOProcess, (void *) CPU->IO1);
	pthread_create(&(CPU->IO2_thread), NULL, RunIOProcess, (void *) CPU->IO2);
	pthread_create(&(CPU->Keyboard_thread), NULL, KBDevRun, (void *) CPU->Keyboard);

	CPU->runningPCB = dequeue(CPU->ReadyQueue);
	int interruptOccurred = 0; //0 for interrupt has occurred, 1 for interrupt did not occur
	int callTimerInt = 1; //0 for calling, 1 for not

	while(CPU->count >0) {
		int runForCount;

		if(interruptOccurred == 0) {
			if(CPU->runningPCB->curr_count > WAIT_TIME)
				{
					runForCount = WAIT_TIME;
					callTimerInt = 0;
				}
				else
				{
					runForCount = CPU->runningPCB->curr_count;
				}
		}



		while(runForCount > 0)
		{
			if(TIMERINT) {
				InterruptHandler( 7, CPU->runningPCB);
				//pthread_cond_signal(&CPU->timer);
			}
			if(IO1INT) {
				InterruptHandler( 3, CPU->IO1->owner);
				//pthread_cond_signal(&CPU->IO1Comp);
			}
			if(IO2INT) {
						InterruptHandler( 4, CPU->IO2->owner);
						//pthread_cond_signal(&CPU->IO2Comp);
					}
			/*if(CPU->IOReq) {
						InterruptHandler(1, CPU->runningPCB);
					}*/

			//if(CPU->KbComp) {
						//InterruptHandler( 5, CPU->Keyboard->owner);
						//pthread_cond_signal(&CPU->Keyboard->reset);
					//}
			if(KBINT) {
						InterruptHandler( 5, CPU->runningPCB);
					}

			if(CPU->runningPCB->process->proc_type == 1)
			{
				int i;
				for(i = 0; i < CPU->runningPCB->process->no_requests; i++)
				{
					if(runForCount == CPU->runningPCB->process->requests[i])
					{
						InterruptHandler( 1, CPU->runningPCB);
						interruptOccurred = 0;
					}
				}
			}
			else if(CPU->runningPCB->process->proc_type == 2)
			{
				InterruptHandler( 2, CPU->runningPCB);
				interruptOccurred = 0;
			}
			else if(CPU->runningPCB->process->proc_type == 3 || CPU->runningPCB->process->proc_type == 4)
			{
				InterruptHandler( 6, CPU->runningPCB);
				interruptOccurred = 0;
			}

			runForCount --;
			CPU->count--;

			if(interruptOccurred == 0)
			{
				CPU->runningPCB->curr_count = runForCount;
			}
		}
		if(callTimerInt == 0)
		{
			InterruptHandler(7, CPU->runningPCB);
		}

	}
}

void InterruptHandler(int interrupt, PCBPtr pcbRequest)
{
	switch (interrupt) 
	{
	//if i/o request
	case 1:
		if(aCPU->IO1->IOAvailable == 0)
		{			
			IORequest(aCPU->IO1, aCPU->runningPCB);
			aCPU->runningPCB = deque(aCPU->ReadyQueue);
			aCPU->runningPCB->state = 0;
			//Call IO1 and pass pcbRequest
		}
		else
		{
			if(aCPU->IO2->IOAvailable == 0)
			{
				//io2Free = False;
				//Call IO2 device and pass pcbRequest
				IORequest(aCPU->IO2, aCPU->runningPCB);
				aCPU->runningPCB = deque(aCPU->ReadyQueue);
				aCPU->runningPCB->state = 0;
			}
			else
			{
				enqueue(aCPU->IoQueue, pcbRequest);
			}
		}
	break;
	//if keyboard request
	case 2:	
		if(aCPU->Keyboard->owner == NULL)
		{
			//enqueue(BlockedQueue, pcbRequest);
			aCPU->Keyboard->owner = aCPU->runningPCB;
			aCPU->runningPCB->state = 3;
			aCPU->runningPCB = deque(aCPU->ReadyQueue);
		}
		else
		{
			enqueue(aCPU->KeyboardQueue, pcbRequest);
		}
	break;
	//if i/o 1 complete
	case 3:
		pcbRequest->state = 1;
		enqueue(aCPU->ReadyQueue, pcbRequest);

		if(aCPU->IoQueue->count != 0)
		{
			PCBPtr pcbPtr = deque(aCPU->IoQueue);
			IORequest(aCPU->IO1, pcbPtr);
			//Call IO1 device and pass pcbPtr
		}
	break;
	//if i/o 2 complete
	case 4:		
		pcbRequest->state = 1;
		enqueue(aCPU->ReadyQueue, pcbRequest);

		if(aCPU->IoQueue->count != 0)
		{
			PCBPtr pcbPtr = deque(aCPU->IoQueue);
			IORequest(aCPU->IO2, pcbPtr);
			//Call IO1 device and pass pcbPtr
		}
	break;
	//if keyboard complete
	case 5:
		//PCBPtr pcb = dequeue(BlockedQueue);
		enque(aCPU->ReadyQueue, pcbRequest);
		
		if(aCPU->KeyboardQueue->count != 0)
		{
			PCBPtr CPUKbOwner  = deque(aCPU->KeyboardQueue);
			aCPU->Keyboard->owner = CPUKbOwner;
		}
	break;
	//if  producer or consumer request for mutex
	case 6:
		if (pthread_mutex_trylock(&mutex[pcbRequest->sharedMemInd]) == 0) {
			SetOwner((aCPU->Mutex)[aCPU->runningPCB->sharedMemInd], aCPU->runningPCB);
			aCPU->runningPCB = dequeue(aCPU->ReadyQueue);
			aCPU->runningPCB->state = 0;
		} else {
			pcbRequest->state = 3;
			((mutex[aCPU->runningPCB->sharedMemInd])->waitingPCB) = pcbRequest;
		}
	break;
	//if its a timer request
	case 7:
		enque(aCPU->ReadyQueue, aCPU->runningPCB);
		aCPU->runningPCB = dequeue(aCPU->ReadyQueue);
		aCPU->runningPCB->state = 0;
		//Pass pcb to CPU
	break;
	default:
		printf("Wrong Input");
	break;
	}
}
