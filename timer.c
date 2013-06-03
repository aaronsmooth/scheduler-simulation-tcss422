/*
 * timer.c
 *
 *  Created on: Jun 1, 2013
 *      Author: Toshiba Home
 */


/*
 * timer.c
 *
 *  Created on: May 27, 2013
 *      Author: Aaron Nelson
 */

#include "timer.h"

#define QUANTA 1

TimerPtr TimerConstructor() {
	TimerPtr theTimer = (TimerPtr) malloc(sizeof(Timer));
	//theTimer->mut = PTHREAD_MUTEX_INITIALIZER;
	//theTimer->reset = PTHREAD_COND_INITIALIZER;

	//pthread_create(&theTimer->clockThread, NULL, RunTimer, (void *) theTimer);
	theTimer->anumber = 0;
	return theTimer;
}

void TimerDestructor(TimerPtr this) {
	//pthread_cancel(this->clockThread);
	free(this);
}

void *RunTimer(void *arg) {
	TimerPtr atimer = (TimerPtr) arg;

	while(1) {
		if (TIMERINT == 0) {
			sleep(QUANTA);
			TIMERINT = 1;
			//InterruptHandler(7, NULL);
			//pthread_cond_wait(&atimer->reset, &atimer->mut);
		}
	}
}
/*
int main(argc, argv) {


}*/