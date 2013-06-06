#ifndef CONDITION_VARIABLE_H
#define CONDITION_VARIABLE_H
#include "process.h"
#include "MutexQueue.h"

typedef struct ConVar {
	QueuePtr pcbQueue;
	MtxQueuePtr mutexQueue;
} CVStr;

typedef CVStr *CVPtr;

CVPtr CVConstructor(void);
void CVDestructor(CVPtr);

#endif

