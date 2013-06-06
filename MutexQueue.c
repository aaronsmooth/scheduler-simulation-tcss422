#include "MutexQueue.h"

//construct the empty queue
MtxQueuePtr MutexQueueConstructor()
{
	MtxQueuePtr m = (MtxQueuePtr) malloc(sizeof(MtxQueue));
	
	m->count = 0;
	m->first = 0;
	m->last = 0;
	
	return m;
}

//destroys the queue
void MutexQueueDestruct(MtxQueuePtr this)
{
	free(this);
}

//add a mutex pointer to the queue
void MutexEnqueue(MtxQueuePtr mq, mutexPtr mtx)
{
	mq->mq[ mq->last ] = mtx;
	mq->last = (mq->last + 1) % QUEUE_SIZE;
	mq->count = mq->count + 1;
}

//remove a mutex pointer from the queue, returns the pointer.
mutexPtr MutexDequeue(MtxQueuePtr this)
{
	mutexPtr x = mutexConstructor(-1);
	if (this->first != this->last) {
		x = this->mq[ this->first ];
		this->first = (this->first + 1) % QUEUE_SIZE;
		this->count = this->count - 1;
	}
	return x;
}

//prints only the currently waiting mutex's in the queue.
void MutexPrintQueue(MtxQueuePtr this)
{
	if ((this->first != this->last)) {
		int i;
		int j = this->last;
		mutexPtr mtx = this->mq[this->first];
		printf("P%d", mtx->mutexID);
		for (i = this->first + 1; i < j; i++) {
			MtxQueue current_queue = *this;
			mutexPtr aMutexPtr = current_queue.mq[i];
			printf(", P%d", aMutexPtr->mutexID);
		}
	}
}
