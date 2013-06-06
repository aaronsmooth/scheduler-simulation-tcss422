/*
cccc * Keyboard.c
 *
 *  Created on: May 30, 2013
 *      Author: Aaron Nelson, Ankita Tiku, Michael Carr
 */

#ifndef KEYBOARD_C_
#define KEYBOARD_C_

#include "global.h"
//#include "graphics.h"
#include <windows.h>
#include "Keyboard.h"
#include <stdio.h>
#include <stdlib.h>
//#include <conio.h>

KBDevPtr KBDevConstructor() {
	KBDevPtr keyboard = (KBDevPtr) malloc(sizeof(KBDev));

	keyboard->keyboardFree = 0;
	//keyboard->reset = PTHREAD_COND_INITIALIZER;
	//keyboard->mutex = PTHREAD_MUTEX_INITIALIZER;
	keyboard->owner = NULL;
	keyboard->Blocked = QueueConstructor();

	//pthread_create(&keyboard->keyboard_thread, NULL, KBDevRun, (void *) keyboard);

	return keyboard;
}

void KBDevDestructor(KBDevPtr this) {
	//pthread_cancel(this->keyboard_thread);
	//free(this);
}

void *KBDevRun(void *args) {
	KBDevPtr aKB = (KBDevPtr) args;

	int loopctrl = 1;
	while(loopctrl) {
		if(KBHASPCB) {
		printf("\nA key was pressed");
		getchar();
		KBINT = 1;
		InterruptHandler(5, aKB->owner);
		}
		sleep(1);
	}
	pthread_exit(NULL);

}

#endif /* KEYBOARD_C_ */
/*
int main(argc, argv) {

	}*/