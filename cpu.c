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