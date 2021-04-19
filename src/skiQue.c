#include "skiContainor.h"
#include "sched.h"

static char skiQueID;

typedef struct _circleQue{
	size_t dataSize;
	size_t capacity;
	skiIndex_t rIdx, wIdx;
	void* headID;
	char data[0];
}CclQue_t;

#define __identifyHead(_pHead) ((_pHead) && ((CclQue_t*)_pHead)->headID == &skiQueID)

#define memset __builtin_memset
#define memcpy __builtin_memcpy

#define ATMC_CSA_(_v, _o, _n)	__sync_bool_compare_and_swap(&(_v), _o, _n)

skiHandler_t skiQue_create(size_t dataSize, unsigned char power)
{
	if(power > sizeof(size_t)*8)goto cque_failed;
	size_t capacity = 1 << power;
	CclQue_t* que = malloc(sizeof(CclQue_t) + (dataSize+1)*capacity);
	if(que == NULL)goto cque_failed;

	memset(que->data, 0, (dataSize+1)*capacity);
	que->dataSize = dataSize;
	que->capacity = capacity - 1;
	que->rIdx = que->wIdx = 0;
	que->headID = &skiQueID;

	return que;

cque_failed:
	return NULL;
}

void skiQue_destroy(skiHandler_t handler)
{
	if(!__identifyHead(handler))goto cque_failed;
	CclQue_t* que = handler;
	que->headID = NULL;

	free(handler);
cque_failed:
	return;
}

size_t skiQue_push(skiHandler_t handler, void* data)
{
	if(!__identifyHead(handler))goto cque_failed;
	CclQue_t* que = handler;
	skiIndex_t rIdx = 0;
	skiIndex_t wIdx = 0;
	char* qData = NULL;

	do{
		rIdx = que->rIdx;
		wIdx = que->wIdx;
		qData = que->data + (wIdx&que->capacity) * (que->dataSize+1);
		if((rIdx&que->capacity) == (wIdx&que->capacity) && qData[0] == 1)
			goto cque_failed;
	}while(!ATMC_CSA_(que->wIdx, wIdx, wIdx+1));

	while(qData[0] == 1);
	if(data)memcpy(qData + 1, data, que->dataSize);
	else memset(qData + 1, 0, que->dataSize);
	qData[0] = 1;

	return que->dataSize;
cque_failed:
	return 0;
}

size_t skiQue_pop(skiHandler_t handler, void* data)
{
	if(!__identifyHead(handler))goto cque_failed;
	CclQue_t* que = handler;
	skiIndex_t rIdx = 0;
	skiIndex_t wIdx = 0;
	char* qData = NULL;

	do{
		wIdx = que->wIdx;
		rIdx = que->rIdx;
		qData = que->data + (rIdx&que->capacity) * (que->dataSize+1);
		if((rIdx&que->capacity) == (wIdx&que->capacity) && qData[0] == 0)
			goto cque_failed;
	}while(!ATMC_CSA_(que->rIdx, rIdx, rIdx+1));

	while(qData[0] == 0);
	if(data)memcpy(data, qData + 1, que->dataSize);
	qData[0] = 0;

	return que->dataSize;
cque_failed:
	return 0;
}

void skiQue_clear(skiHandler_t handler)
{
	if(!__identifyHead(handler))goto cque_failed;
	CclQue_t* que = handler;
	skiIndex_t rIdx = 0;
	skiIndex_t wIdx = 0; 
	char* qData = NULL;

	do{
		rIdx = que->rIdx;
		wIdx = que->wIdx; 
	}while(!ATMC_CSA_(que->rIdx, rIdx, wIdx));

	do{
		qData = que->data + (rIdx&que->capacity) * (que->dataSize+1);
		//if(qData[0] == 1)qData[0] = 0;
		ATMC_CSA_(qData[0], 1, 0);
	}while(rIdx++ != wIdx);

cque_failed:
	return;
}


size_t skiQue_poll(skiHandler_t handler, char rw)
{
	if(!__identifyHead(handler))goto cque_failed;
	CclQue_t* que = handler;
	skiIndex_t rIdx = que->rIdx;
	skiIndex_t wIdx = que->wIdx; 
	char* qData = que->data + (rIdx&que->capacity) * (que->dataSize+1);

	while((rIdx&que->capacity) == (wIdx&que->capacity) && qData[0] == rw){
		sched_yield();
		rIdx = que->rIdx;
		wIdx = que->wIdx; 
		qData = que->data + (rIdx&que->capacity) * (que->dataSize+1);
	}

	if((rIdx&que->capacity) == (wIdx&que->capacity))return qData[0]*(que->capacity+1);
	else return (wIdx - rIdx)&que->capacity;
cque_failed:
	return 0;
}











