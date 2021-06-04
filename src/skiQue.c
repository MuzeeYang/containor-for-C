#include "skiQue.h"

static char skiQueID;

typedef struct _circleQue{
	size_t dataSize;
	size_t capacity;
	skiIndex_t rIdx, wIdx;
	//skiIndex_t pushIdx, popIdx;
	void* headID;
	char data[0];
}CclQue_t;

#define __identifyHead(_pHead) ((_pHead) && ((CclQue_t*)_pHead)->headID == &skiQueID)


skiHandler_t skiQue_create(size_t dataSize, unsigned char power)
{
	if(power > sizeof(size_t)*8)goto cque_failed;
	size_t capacity = 1 << power;
	CclQue_t* que = malloc(sizeof(CclQue_t) + (dataSize+1)*capacity);
	if(que == NULL)goto cque_failed;

	memset(que->data, 0, (dataSize)*capacity);
	que->dataSize = dataSize;
	que->capacity = capacity - 1;
	//que->readIdx = que->writeIdx = 0;
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

size_t skiQue_push(skiHandler_t handler, void* data, skiFunc1_t pauseFunc, void* arg)
{
	if(!__identifyHead(handler))goto cque_failed;
	CclQue_t* que = handler;
	skiIndex_t rIdx = 0;
	skiIndex_t wIdx = 0;
	char* qData = NULL;

	do{
cque_resume:
		rIdx = que->rIdx;
		wIdx = que->wIdx;
		qData = que->data + (wIdx&que->capacity)*(que->dataSize+1);
		if((rIdx&que->capacity) == (wIdx&que->capacity) && qData[0] == 1){
			if(pauseFunc){
				pauseFunc(arg);
				goto cque_resume;
			}else goto cque_failed;
		}
	}while(!SKI_ATMC_CSA_(que->wIdx, wIdx, wIdx+1));

	while(qData[0] == 1);
	if(data)memcpy(qData + 1, data, que->dataSize);
	SKI_ATMC_CSA_(qData[0], 0, 1);

	return que->capacity;
cque_failed:
	return 0;
}

size_t skiQue_pop(skiHandler_t handler, void* data, skiFunc1_t pauseFunc, void* arg)
{
	if(!__identifyHead(handler))goto cque_failed;
	CclQue_t* que = handler;
	skiIndex_t rIdx = 0;
	skiIndex_t wIdx = 0;
	char* qData = NULL;

	do{
cque_resume:
		wIdx = que->wIdx;
		rIdx = que->rIdx;
		qData = que->data + (rIdx&que->capacity)*(que->dataSize+1);
		if((rIdx&que->capacity) == (wIdx&que->capacity) && qData[0] == 0){
			if(pauseFunc){
				pauseFunc(arg);
				goto cque_resume;
			}else goto cque_failed;
		}
	}while(!SKI_ATMC_CSA_(que->rIdx, rIdx, rIdx+1));

	while(qData[0] == 0);
	if(data)memcpy(data, qData + 1, que->dataSize);
	SKI_ATMC_CSA_(qData[0], 1, 0);

	return que->capacity;
cque_failed:
	return 0;
}

void skiQue_clear(skiHandler_t handler)
{
	while(skiQue_pop(handler, NULL, NULL, NULL));
}

size_t skiQue_size(skiHandler_t handler)
{
	if(!__identifyHead(handler))goto cque_failed;
	CclQue_t* que = handler;
	skiIndex_t rIdx = que->rIdx;
	skiIndex_t wIdx = que->wIdx;
	char* qData = que->data + (rIdx&que->capacity)*(que->dataSize+1);

	if((rIdx&que->capacity) == (wIdx&que->capacity))return (que->capacity+1)*qData[0];
	else return (wIdx - rIdx) & que->capacity;
cque_failed:
	return 0;
}

