/*
 *	cpu.c
 *	
 *	Created on: 5/29/2013
 *	Author:	Michael Carr
 */
 #include <stdio.h>
 #include <pthread.h>
 #include "global.h"
 #include "process.h"
 #include "pcb.h"
 #include "Queue.h"
 #include "IODevice.h"
 #include "Keyboard.h"
 #include "mutex.h"
 #include "cpu.h"

cpuPtr cpuConstructor(QueuePtr ReadyQ, int memLocations, mutexPtr mutexArray)
{
	//create CPU with random count between 1000 and 10000
	cpuPtr q = (cpuPtr) malloc(sizeof(cpuObj));
	q->timerINT = 0;
	q->schedulerReady = 0;
	q->schedulerQueue = QueueConstructor();
	q->isRunning = FALSE;
	q->count = random_in_range( 1000, 10000 );
	q->memoryLocations = memLocations;
	q->IoQueue = QueueConstructor();
	q->IO1 = IOConstructor(1, q);
	q->IO2 = IOConstructor(2, q);
	q->Keyboard = KBDevConstructor();
	q->KeyboardQueue = QueueConstructor();
	q->Mutex = mutexArray;
	q->runCount = 0;
	q->runningPCB = NULL;
	//create the ready queue
	q->ReadyQueue = ReadyQ;
	q->switchcomplete = PTHREAD_COND_INITIALIZER;
	q->timer = PTHREAD_COND_INITIALIZER;
	q->keyboarddone = PTHREAD_COND_INITIALIZER;
	q->mutex = PTHREAD_MUTEX_INITIALIZER;
	q->count = 1000;
	//create the 
	
	//pthread_attr_t attr;
    //pthread_attr_init(&attr);
    //pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
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

void *CPURun(cpuPtr CPU) {
	CPU->runningPCB = dequeue(CPU->ReadyQueue);
	int interruptOccurred = 0; //0 for interrupt has occurred, 1 for interrupt did not occur
			int callTimerInt = 1; //0 for calling, 1 for not

	while(CPU->count >0) {
		int runForCount;

		if(interruptOccurred == 0)
		{
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
		if(CPU->timerINT) {
			InterruptHandler(7, CPU->runningPCB);
			pthread_cond_signal(&CPU->timer);
		}
		if(CPU->IO1Comp) {
			InterruptHandler(3, CPU->IO1->owner);
			pthread_cond_signal(&CPU->IO1Comp);
		}
		if(CPU->IO2Comp) {
					InterruptHandler(4, CPU->IO2->owner);
					pthread_cond_signal(&CPU->IO2Comp);
				}
		/*if(CPU->IOReq) {
					InterruptHandler(1, CPU->runningPCB);
				}*/
		if(CPU->KbComp) {
					InterruptHandler(5, CPU->Keyboard->owner);
					pthread_cond_signal(&CPU->Keyboard->reset);
				}
		if(CPU->KbReq) {
					InterruptHandler(2, CPU->runningPCB);
				}

		if(CPU->runningPCB->process->proc_type == 1)
		{
			for(int i; i < CPU->runningPCB->process->no_requests; i++)
			{
				if(runForCount == CPU->runningPCB->process->requests[i])
				{
					InterruptHandler(1, CPU->runningPCB);
					interruptOccurred = 0;
				}
			}
		}
		else if(CPU->runningPCB->process->proc_type == 2)
		{
			InterruptHandler(2, CPU->runningPCB);
			interruptOccurred = 0;
		}
		else if(CPU->runningPCB->process->proc_type == 3 || CPU->runningPCB->process->proc_type == 4)
		{
			InterruptHandler(6, CPU->runningPCB);
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

void InterruptHandler(int interrupt, PCBPtr pcbRequest)
{
	switch (interrupt) 
	{
	//if i/o request
	case 1:
		if(CPU->IO1->IOAvailable == 0)
		{			
			IORequest(CPU->IO1, CPU->runningPCB);
			CPU->runningPCB = deque(CPU->ReadyQueue);
			CPU->runningPCB->state = 0;
			//Call IO1 and pass pcbRequest
		}
		else
		{
			if(CPU->IO2->IOAvailable == 0)
			{
				//io2Free = False;
				//Call IO2 device and pass pcbRequest
				IORequest(CPU->IO2, CPU->runningPCB);
				CPU->runningPCB = deque(CPU->ReadyQueue);
				CPU->runningPCB->state = 0;
			}
			else
			{
				enqueue(CPU->IoQueue, pcbRequest);
			}
		}
	break;
	//if keyboard request
	case 2:	
		if(CPU->Keyboard->owner == NULL)
		{
			//enqueue(BlockedQueue, pcbRequest);
			CPU->Keyboard->owner = CPU->runningPCB;
			CPU->runningPCB->state = 3;
			CPU->runningPCB = deque(CPU->ReadyQueue);
		}
		else
		{
			enqueue(CPU->KeyboardQueue, pcbRequest);
		}
	break;
	//if i/o 1 complete
	case 3:
		pcbRequest->state = 1;
		enqueue(CPU->ReadyQueue, pcbRequest);

		if(CPU->IoQueue->count != 0)
		{
			PCBPtr pcbPtr = deque(CPU->IoQueue);
			IORequest(CPU->IO1, pcbPtr);
			//Call IO1 device and pass pcbPtr
		}
	break;
	//if i/o 2 complete
	case 4:		
		pcbRequest->state = 1;
		enqueue(CPU->ReadyQueue, pcbRequest);

		if(CPU->IoQueue->count != 0)
		{
			PCBPtr pcbPtr = deque(CPU->IoQueue);
			IORequest(CPU->IO2, pcbPtr);
			//Call IO1 device and pass pcbPtr
		}
	break;
	//if keyboard complete
	case 5:
		//PCBPtr pcb = dequeue(BlockedQueue);
		enque(CPU->ReadyQueue, pcbRequest);
		
		if(CPU->KeyboardQueue->count != 0)
		{
			PCBPtr CPUKbOwner  = deque(CPU->KeyboardQueue);
			CPU->Keyboard->owner = CPUKbOwner;
		}
	break;
	//if  producer or consumer request for mutex
	case 6:
		SetOwner((CPU->Mutex)[CPU->runningPCB->sharedMemInd], CPU->runningPCB);
		CPU->runningPCB = dequeue(CPU->ReadyQueue);
		CPU->runningPCB->state = 0;
	break;
	//if its a timer request
	case 7:
		enque(CPU->ReadyQueue, pcbRequest);
		CPU->runningPCB = dequeue(CPU->ReadyQueue);
		CPU->runningPCB->state = 0;
		//Pass pcb to CPU
	break;
	default:
		printf("Wrong Input");
	break;
	}
}
