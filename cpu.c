/*
 *	cpu.c
 *	
 *	Created on: 5/29/2013
 *	Author:	Michael Carr
 */
 #include "global.h"

cpuPtr cpuConstructor() 
{
	mutexCpu q = (cpuPtr) malloc(sizeof(cpuObj));
	q->isRunning = FALSE;
	q->count = random_in_range( 1000, 10000 );
	pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
	pthread_create(&(toReturn->threads[t++]), &attr, Timer, (void *)toReturn);
    pthread_attr_destroy(&attr); 
	return q;
}

/* Would like a semi-open interval [min, max) */
int random_in_range (unsigned int min, unsigned int max)
{
  int base_random = rand(); /* in [0, RAND_MAX] */
  if (RAND_MAX == base_random) return random_in_range(min, max);
  /* now guaranteed to be in [0, RAND_MAX) */
  int range       = max - min,
      remainder   = RAND_MAX % range,
      bucket      = RAND_MAX / range;
  /* There are range buckets, plus one smaller interval
     within remainder of RAND_MAX */
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
}

void InterruptHandler(int interrupt, PCBPtr pcbRequest)
{
	switch (interrupt) 
	{
	//if i/o request
	case 1:
		if(io1Free == True)
		{			
			io1Free = False;
			//Call IO1 and pass pcbRequest
		}
		else
		{
			if(io2Free == True)
			{
				io2Free = False;				
				//Call IO2 device and pass pcbRequest
			}
			else
			{
				enqueue(Ioqueue, pcbRequest);
			}
		}
	break;
	//if keyboard request
	case 2:	
		if(keyboardFree == True
		{
			enqueue(BlockedQueue, pcbRequest);
		}
		else
		{
			enqueue(KeyboardQueue, pcbRequest);
		}
	break;
	//if i/o 1 complete
	case 3:		
		enqueue(ReadyQueue, pcbRequest)

		if(Ioqueue->first != Ioqueue->last)
		{
			PCBPtr pcbPtr = deque(Ioqueue);			
			//Call IO1 device and pass pcbPtr
		}
		else
		{
			io1Free = True;
		}
	break;
	//if i/o 2 complete
	case 4:		
		enqueue(ReadyQueue, pcbRequest)

		if(Ioqueue->first != Ioqueue->last)
		{
			PCBPtr pcbPtr = dequeu(Ioqueue);			
			//Call IO2 device and pass pcbPtr
		}
		else
		{
			io2Free = True;
		}
	break;
	//if keyboard complete
	case 5:
		PCBPtr pcb = dequeue(BlockedQueue);
		enque(ReadyQueue, pcb);
		
		if(KeyboardQueue->first != KeyboardQueue->last)
		{
			PCBPtr pcbPtr  = deque(KeyboardQueue);
			enqueue(BlockedQueue, pcbPtr);

		}
	break;
	//if  producer or consumer request for mutex
	case 6:
		if(sharedMemoryFree == True)
		{
			sharedMemoryFree = False;
			//Call Mutex Handler
		}
		else
		{
			enque(MutexQueue, pcbRequest);
		}
	break;
	//if its a timer request
	case 7:
		enque(ReadyQueue, pcbRequest);
		PCBPtr pcb = dequeue(ReadyQueue);
		//Pass pcb to CPU
	break;
	default:
		printf("Wrong Input");
	break;
	}
}