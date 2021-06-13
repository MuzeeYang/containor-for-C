#include "skiQue.h"

static char skiQueID;

typedef struct _circleQue{
	size_t dataSize;
	size_t capacity;
	volatile skiIndex_t rdIdx, wtIdx;
	skiIndex_t pushIdx, popIdx;
	void* headID;
	char data[0];
}skiQue_t;

#define __identifyHead(_pHead) ((_pHead) && ((skiQue_t*)_pHead)->headID == &skiQueID)


skiHandler_t skiQue_create(size_t dataSize, unsigned char power)
{
	if(power > sizeof(size_t)*8)goto cque_failed;
	size_t capacity = 1 << power;
	skiQue_t* que = malloc(sizeof(skiQue_t) + (dataSize)*capacity);
	if(que == NULL)goto cque_failed;

	memset(que, 0, sizeof(skiQue_t) + (dataSize)*capacity);
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
	skiQue_t* que = handler;
	que->headID = NULL;

	free(handler);
cque_failed:
	return;
}

size_t skiQue_push(skiHandler_t handler, void* data, skiFuncPause_t pauseFunc, void* arg)
{
	if(!__identifyHead(handler))goto cque_failed;
	skiQue_t* que = handler;
	skiIndex_t rdIdx = 0;
	skiIndex_t pushIdx = 0;
	size_t cnt = 0;

	do{
cque_resume:
		rdIdx = que->rdIdx;
		pushIdx = que->pushIdx;
		//printf("%s: rdIdx = %lu, pushIdx = %lu, size = %d, capacity = %lu\n", __func__, rdIdx, pushIdx, pushIdx-rdIdx, que->capacity);
		if(pushIdx - rdIdx >= que->capacity){
			if(pauseFunc){
				pauseFunc(&cnt, arg);
				cnt++;
				goto cque_resume;
			}else goto cque_failed;
		}
	}while(!SKI_ATMC_CSA_(que->pushIdx, pushIdx, pushIdx+1));

	if(data)memcpy(que->data + (pushIdx&que->capacity)*que->dataSize, data, que->dataSize);
	else memset(que->data + (pushIdx&que->capacity)*que->dataSize, 0, que->dataSize);
	while(!SKI_ATMC_CSA_(que->wtIdx, pushIdx, pushIdx+1));

	return pushIdx - rdIdx;
cque_failed:
	return 0;
}

size_t skiQue_pop(skiHandler_t handler, void* data, skiFuncPause_t pauseFunc, void* arg)
{
	if(!__identifyHead(handler))goto cque_failed;
	skiQue_t* que = handler;
	skiIndex_t popIdx = 0;
	skiIndex_t wtIdx = 0;
	size_t cnt = 0;

	do{
cque_resume:
		wtIdx = que->wtIdx;
		popIdx = que->popIdx;
		//printf("%s: popIdx = %lu, wtIdx = %lu, size = %d, capacity = %lu\n", __func__, popIdx, wtIdx, wtIdx-popIdx, que->capacity);
		if(popIdx == wtIdx){
			if(pauseFunc){
				pauseFunc(&cnt, arg);
				cnt++;
				goto cque_resume;
			}else goto cque_failed;
		}
	}while(!SKI_ATMC_CSA_(que->popIdx, popIdx, popIdx+1));

	if(data)memcpy(data, que->data + (popIdx&que->capacity)*que->dataSize, que->dataSize);
	while(!SKI_ATMC_CSA_(que->rdIdx, popIdx, popIdx+1));

	return wtIdx - popIdx;
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
	skiQue_t* que = handler;
	return que->wtIdx - que->rdIdx;
cque_failed:
	return 0;
}

