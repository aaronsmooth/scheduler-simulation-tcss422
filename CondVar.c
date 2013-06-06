#include "CondVar.h"

CVPtr CVConstructor(void)
{
	CVPtr c = (CVPtr) malloc(sizeof(CVStr));
	c->pcbQueue = QueueConstructor();
	c->mutexQueue = MutexQueueConstructor();
	c->isWaiting = 0;
	return c;
}

void CVDestructor(CVPtr this)
{
	QueueDestruct(this->pcbQueue);
	MutexQueueDestruct(this->mutexQueue);
	free(this);
}