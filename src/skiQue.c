#include "skiQue.h"

static char skiQueID;

typedef struct _circleQue{
	size_t dataSize;
	size_t capacity;
	skiIndex_t readIdx, writeIdx;
	skiIndex_t pushIdx, popIdx;
	void* headID;
	char data[0];
}CclQue_t;

#define __identifyHead(_pHead) ((_pHead) && ((CclQue_t*)_pHead)->headID == &skiQueID)


skiHandler_t skiQue_create(size_t dataSize, unsigned char power)
{
	if(power > sizeof(size_t)*8)goto cque_failed;
	size_t capacity = 1 << power;
	CclQue_t* que = malloc(sizeof(CclQue_t) + (dataSize)*capacity);
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
	skiIndex_t readIdx = 0;
	skiIndex_t pushIdx = 0;

	do{
cque_resume:
		readIdx = que->readIdx;
		pushIdx = que->pushIdx;
		if((readIdx&que->capacity) == ((pushIdx+1)&que->capacity)){
			if(pauseFunc){
				pauseFunc(arg);
				goto cque_resume;
			}else goto cque_failed;
		}
	}while(!SKI_ATMC_CSA_(que->pushIdx, pushIdx, pushIdx+1));

	if(data)memcpy(que->data + (pushIdx&que->capacity)*que->dataSize, data, que->dataSize);
	while(!SKI_ATMC_CSA_(que->writeIdx, pushIdx, pushIdx+1));

	return que->capacity;
cque_failed:
	return 0;
}

size_t skiQue_pop(skiHandler_t handler, void* data, skiFunc1_t pauseFunc, void* arg)
{
	if(!__identifyHead(handler))goto cque_failed;
	CclQue_t* que = handler;
	skiIndex_t popIdx = 0;
	skiIndex_t writeIdx = 0;

	do{
cque_resume:
		writeIdx = que->writeIdx;
		popIdx = que->popIdx;
		if((popIdx&que->capacity) == (writeIdx&que->capacity)){
			if(pauseFunc){
				pauseFunc(arg);
				goto cque_resume;
			}else goto cque_failed;
		}
	}while(!SKI_ATMC_CSA_(que->popIdx, popIdx, popIdx+1));

	if(data)memcpy(data, que->data + (popIdx&que->capacity)*que->dataSize, que->dataSize);
	while(!SKI_ATMC_CSA_(que->readIdx, popIdx, popIdx+1));

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

	return (que->writeIdx - que->readIdx) & que->capacity;
cque_failed:
	return 0;
}

