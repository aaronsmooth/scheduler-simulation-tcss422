/*
 * timer.h
 *
 *  Created on: Jun 1, 2013
 *      Author: Toshiba Home
 */

#ifndef TIMER_H_
#define TIMER_H_


#include <stdio.h>
#include <stdlib.h>
#include "global.h"

typedef struct {
	//pthread_t clockThread;
	//pthread_cond_t reset;
	//pthread_mutex_t mut;
	int anumber;
} Timer;

typedef Timer *TimerPtr;

TimerPtr TimerConstructor();

void TimerDestructor(TimerPtr);

void *RunTimer(void *);

#endif
