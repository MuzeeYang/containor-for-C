#include "skiBuffer.h"

static char skiBufID;

typedef struct _ski_buf{
	size_t capacity;
	size_t size;
	void* headID;
	char* buf;
}BufVector_t;


#define __identifyHead(_pHead) ((_pHead) && ((BufVector_t*)_pHead)->headID == &skiBufID)

static size_t inline __increase_size(size_t orgSize)
{
	return orgSize + (orgSize > 1);
}

skiHandler_t skiBuffer_create(void* buf, size_t length)
{
	size_t capacity = length;
	if(capacity < 16)capacity = 16;
	BufVector_t* pBufVct = malloc(sizeof(BufVector_t));
	if(pBufVct == NULL)goto buf_failed;

	pBufVct->buf = malloc(capacity);
	if(pBufVct->buf == NULL)goto buf_failed;

	pBufVct->capacity = capacity;
	if(buf){
		memcpy(pBufVct->buf, buf, length);
		memset(pBufVct->buf + length, 0, capacity - length);
		pBufVct->size = length;
	}else{
		memset(pBufVct->buf, 0, capacity);
		pBufVct->size = 0;
	} 
	pBufVct->headID = &skiBufID;

	return pBufVct;
buf_failed:
	if(pBufVct)free(pBufVct);
	return NULL;
}

void skiBuffer_destroy(skiHandler_t handler)
{
	if(!__identifyHead(handler))goto buf_failed;
	BufVector_t* pBufVct = handler;

	if(pBufVct->buf)free(pBufVct->buf);
	pBufVct->headID = NULL;
	free(pBufVct);

buf_failed:
	return;
}

skiHandler_t skiBuffer_copy(skiHandler_t handler)
{
	if(!__identifyHead(handler))goto buf_failed;
	BufVector_t* pBufVct = handler;
	BufVector_t* copy = NULL;

	copy = skiBuffer_create(pBufVct->buf, pBufVct->size);
	if(copy == NULL)goto buf_failed;

	return copy;
buf_failed:
	return NULL;
}

static size_t _recap_bufvct(BufVector_t* pBufVct, size_t reqSize)
{
	size_t newCap = pBufVct->capacity;
	do{
		newCap = __increase_size(newCap);
	}while(reqSize > newCap);
	char* newBuf = malloc(newCap);
	if(newBuf == NULL)return 0;

	memcpy(newBuf, pBufVct->buf, pBufVct->size);
	memset(newBuf + pBufVct->size, 0, newCap - pBufVct->size);
	free(pBufVct->buf);
	pBufVct->buf = newBuf;
	pBufVct->capacity = newCap;
	return newCap;
}

size_t skiBuffer_resize(skiHandler_t handler, size_t size)
{
	if(!__identifyHead(handler))goto buf_failed;
	BufVector_t* pBufVct = handler;

	if(size > pBufVct->capacity){
		if(0 == _recap_bufvct(pBufVct, size))goto buf_failed;
	}

	if(size < pBufVct->size)memset(pBufVct->buf + size, 0, pBufVct->size - size);
	pBufVct->size = size;

	return pBufVct->size;
buf_failed:
	return 0;
}

size_t skiBuffer_insert(skiHandler_t handler, skiIndex_t offset, void* buf, size_t length)
{
	if(!__identifyHead(handler) || !buf)goto buf_failed;
	BufVector_t* pBufVct = handler;

	if(pBufVct->size + length > pBufVct->capacity){
		if(0 == _recap_bufvct(pBufVct, pBufVct->size + length))goto buf_failed;
	}

	if(offset > pBufVct->size)offset = pBufVct->size;
	memmove(pBufVct->buf + offset + length, pBufVct->buf + offset, pBufVct->size - offset);
	memcpy(pBufVct->buf + offset, buf, length);
	pBufVct->size += length;

	return length;
buf_failed:
	return 0;
}

size_t skiBuffer_append(skiHandler_t handler, void* buf, size_t length)
{
	if(!__identifyHead(handler) || !buf)goto buf_failed;
	BufVector_t* pBufVct = handler;

	if(pBufVct->size + length > pBufVct->capacity){
		if(0 == _recap_bufvct(pBufVct, pBufVct->size + length))goto buf_failed;
	}

	memcpy(pBufVct->buf + pBufVct->size, buf, length);
	pBufVct->size += length;
	return length;
	
buf_failed:
	return 0;
}

size_t skiBuffer_cut(skiHandler_t handler, skiIndex_t offset, void* buf, size_t length)
{
	if(!__identifyHead(handler))goto buf_failed;
	BufVector_t* pBufVct = handler;

	if(offset > pBufVct->size)offset = pBufVct->size;
	if(length > pBufVct->size - offset)length = pBufVct->size - offset;
	size_t rest = pBufVct->size - offset - length;

	if(buf)memcpy(buf, pBufVct->buf + offset, length);
	if(rest)memmove(pBufVct->buf + offset, pBufVct->buf + offset + length, rest);
	memset(pBufVct->buf + offset + rest, 0, length);
	pBufVct->size -= length;

	return length;
buf_failed:
	return 0;
}

size_t skiBuffer_truncate(skiHandler_t handler, void* buf, size_t length)
{
	if(!__identifyHead(handler))goto buf_failed;
	BufVector_t* pBufVct = handler;

	if(length > pBufVct->size)length = pBufVct->size;
	size_t offset = pBufVct->size - length;

	if(buf)memcpy(buf, pBufVct->buf + offset, length);
	memset(pBufVct->buf + offset, 0, length);
	pBufVct->size = offset;

	return length;
buf_failed:
	return 0;
}

skiIndex_t skiBuffer_find(skiHandler_t handler, skiIndex_t offset, void* buf, size_t length)
{
	if(!__identifyHead(handler) || !buf)goto buf_failed;
	BufVector_t* pBufVct = handler;

	while(offset + length <= pBufVct->size){
		if(0 == memcmp(pBufVct->buf + offset, buf, length))
			return offset;
		offset++;
	}
buf_failed:
	return pBufVct->size;
}

size_t skiBuffer_replace(skiHandler_t handler, skiIndex_t offset, size_t rpLength, void* buf, size_t bufLength)
{
	if(!__identifyHead(handler) || !buf)goto buf_failed;
	BufVector_t* pBufVct = handler;
	size_t length, rest;

	if(offset > pBufVct->size)offset = pBufVct->size;
	if(rpLength > pBufVct->size - offset)rpLength = pBufVct->size - offset;
	rest = pBufVct->size - offset - rpLength;
	
	if(rpLength < bufLength){
		length = bufLength - rpLength;
		if(pBufVct->size + length > pBufVct->capacity){
			if(0 == _recap_bufvct(pBufVct, pBufVct->size + length))goto buf_failed;
		}

		pBufVct->size += length;
	}else{
		length = rpLength - bufLength;

		if(length)memset(pBufVct->buf + offset + bufLength + rest, 0, length);
		pBufVct->size -= length;
	}

	if(rest)memmove(pBufVct->buf + offset + bufLength, pBufVct->buf + offset + rpLength, rest);
	memcpy(pBufVct->buf + offset, buf, bufLength);

	return bufLength;
buf_failed:
	return 0;
}

char* skiBuffer_at(skiHandler_t handler, skiIndex_t idx)
{
	if(!__identifyHead(handler))goto buf_failed;
	BufVector_t* pBufVct = handler;

	if(idx >= pBufVct->size)goto buf_failed;

	return pBufVct->buf + idx;
buf_failed:
	return NULL;
}

size_t skiBuffer_size(skiHandler_t handler)
{
	if(!__identifyHead(handler))goto buf_failed;
	BufVector_t* pBufVct = handler;

	return pBufVct->size;
buf_failed:
	return 0;
}


