/*
 * Keyboard.h
 *
 *  Created on: May 30, 2013
 *      Author: Aaron Nelson
 */

#ifndef KEYBOARD_H_
#define KEYBOARD_H_
typedef struct {
	int keyboardFree;		//flag that indicates whether the keyboard is free. 0 = free, 1 = being used by another process.
	PCBPtr owner;			//the current holder of the keyboard i/o.
	QueuePtr Blocked[];		//a queue of waiting processes for the keyboard to become available.
	pthread_t keyboard_thread;	//makes the keyboard device a thread
	pthread_cond_t reset;		//cond variable so that the cpu can indicate that the interrupt was serviced.
	pthread_mutex_t mutex;		//unused mutex only for signaling.
} KBDev;

typedef KBDev *KBDevPtr;

//construct a keyboard device
KBDevPtr KBDevConstructor();

//destroys a keyboard device
void KBDevDestructor(KBDevPtr);

//runs a loop that detects when a key is pressed on the keyboard and signals to the cpu when it happens.
void *KBDevRun(void *);

#endif /* KEYBOARD_H_ */
