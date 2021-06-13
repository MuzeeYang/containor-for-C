#include "skiBuffer.h"
#include "skiList.h"

static char skiBufID;

typedef struct _ski_buf{
	size_t capacity;
	size_t size;
	void* headID;
	char* buf;
}skiBuffer_t;


#define __identifyHead(_pHead) ((_pHead) && ((skiBuffer_t*)_pHead)->headID == &skiBufID)

static size_t inline __increase_size(size_t orgSize)
{
	return orgSize + (orgSize >> 1);
}

skiHandler_t skiBuffer_create(void* buf, size_t length)
{
	size_t capacity = length;
	if(capacity < 16)capacity = 16;
	skiBuffer_t* pBufVct = malloc(sizeof(skiBuffer_t));
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
	skiBuffer_t* pBufVct = handler;

	if(pBufVct->buf)free(pBufVct->buf);
	pBufVct->headID = NULL;
	free(pBufVct);

buf_failed:
	return;
}

skiHandler_t skiBuffer_copy(skiHandler_t handler)
{
	if(!__identifyHead(handler))goto buf_failed;
	skiBuffer_t* pBufVct = handler;

	return skiBuffer_create(pBufVct->buf, pBufVct->size);
buf_failed:
	return NULL;
}

static size_t _recap_bufvct(skiBuffer_t* pBufVct, size_t reqSize)
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
	skiBuffer_t* pBufVct = handler;

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
	skiBuffer_t* pBufVct = handler;

	if(pBufVct->size + length >= pBufVct->capacity){
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
	skiBuffer_t* pBufVct = handler;

	if(pBufVct->size + length >= pBufVct->capacity){
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
	skiBuffer_t* pBufVct = handler;

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
	skiBuffer_t* pBufVct = handler;

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
	skiBuffer_t* pBufVct = handler;

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
	skiBuffer_t* pBufVct = handler;
	size_t length, rest;

	if(offset > pBufVct->size)offset = pBufVct->size;
	if(rpLength > pBufVct->size - offset)rpLength = pBufVct->size - offset;
	rest = pBufVct->size - offset - rpLength;
	
	if(rpLength < bufLength){
		length = bufLength - rpLength;
		if(pBufVct->size + length >= pBufVct->capacity){
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
	skiBuffer_t* pBufVct = handler;

	if(idx >= pBufVct->size)goto buf_failed;

	return pBufVct->buf + idx;
buf_failed:
	return NULL;
}

size_t skiBuffer_size(skiHandler_t handler)
{
	if(!__identifyHead(handler))goto buf_failed;
	skiBuffer_t* pBufVct = handler;

	return pBufVct->size;
buf_failed:
	return 0;
}

//================ String ==================
#define SKI_NO_0BYTE_IN32(_v32)	((((_v32) & 0x7F7F7F7F) + 0x7F7F7F7F) & 0x80808080)

static size_t _skiStrlen(char* string)
{
	skiU32_t* strblk = (skiU32_t*)string;
	size_t strlen = 0;
	if(strblk == NULL)goto buf_failed;
	while(SKI_NO_0BYTE_IN32(*strblk))strblk++;

	strlen = (char*)strblk - string;

	string = (char*)strblk;
	while(*string)string++;

	strlen += string - (char*)strblk;
buf_failed:
	return strlen;
}

skiHandler_t skiString_create(char* string)
{
	size_t length = _skiStrlen(string);
	if(length == 0)goto buf_failed;

	skiHandler_t buf = skiBuffer_create(string, length+1);
	if(buf == NULL)goto buf_failed;

	SKI_P2T(buf, skiBuffer_t).size = length;
	return buf;
buf_failed:
	return NULL;
}

void skiString_destroy(skiHandler_t handler)
{
	skiBuffer_destroy(handler);
}

skiHandler_t skiString_copy(skiHandler_t handler)
{
	if(!__identifyHead(handler))goto buf_failed;
	skiBuffer_t* pBufVct = handler;
	skiHandler_t newString = skiString_create(pBufVct->buf);

	return newString;
buf_failed:
	return NULL;
}

const char* skiString_get(skiHandler_t handler)
{
	return (const char*)skiBuffer_at(handler, 0);
}

skiHandler_t skiString_insert(skiHandler_t handler, skiIndex_t idx, char* str)
{
	skiBuffer_insert(handler, idx, str, _skiStrlen(str));
	return handler;
}

skiHandler_t skiString_append(skiHandler_t handler, char* str)
{
	skiBuffer_append(handler, str, _skiStrlen(str));
	return handler;
}

skiHandler_t skiString_cut(skiHandler_t handler, skiIndex_t idx, size_t length)
{
	skiBuffer_cut(handler, idx, NULL, length);
	return handler;
}

skiHandler_t skiString_truncate(skiHandler_t handler, size_t length)
{
	skiBuffer_truncate(handler, NULL, length);
	return handler;
}

skiHandler_t skiString_find(skiHandler_t handler, skiIndex_t idx, char* str)
{
	return skiBuffer_at(handler, skiBuffer_find(handler, idx, str, _skiStrlen(str)));
}

skiHandler_t skiString_replace(skiHandler_t handler, char* oldStr, char* newStr)
{
	size_t oldStrLen = _skiStrlen(oldStr);
	size_t newStrLen = _skiStrlen(newStr);
	skiIndex_t startIdx = 0;
	do{
		startIdx = skiBuffer_find(handler, startIdx, oldStr, oldStrLen);
	}while((startIdx += skiBuffer_replace(handler, startIdx, oldStrLen, newStr, newStrLen)));
	return handler;
}


static int _clear_buffer(void* pHandler)
{
	if(pHandler)skiBuffer_destroy(SKI_P2T(pHandler, skiHandler_t));
	return 1;
}

skiHandler_t skiString_split(skiHandler_t handler, void* buf, size_t length)
{
	if(!__identifyHead(handler) || !buf || !length)goto buf_failed;
	skiBuffer_t* pBufVct = handler;

	skiHandler_t list = skiList_create(sizeof(skiHandler_t));
	if(list == NULL)goto buf_failed;

	skiIndex_t foundIdx = 0;
	skiIndex_t startIdx = 0;
	skiHandler_t newBuffer = NULL;
	for(; startIdx <= pBufVct->size; startIdx = foundIdx + length){
		foundIdx = skiBuffer_find(handler, startIdx, buf, length);
		newBuffer = skiBuffer_create(pBufVct->buf+startIdx, foundIdx-startIdx+1);
		if(newBuffer == NULL){
			skiList_clear(list, _clear_buffer);
			skiList_destroy(list);
			goto buf_failed;
		}
		skiBuffer_resize(pBufVct, foundIdx-startIdx);
		skiList_pushBack(list, &newBuffer);
	}

	return list;
buf_failed:
	return NULL;
}

skiHandler_t skiString_join(skiHandler_t handler, skiHandler_t list)
{
	if(!__identifyHead(handler))goto buf_failed;
	skiBuffer_t* pBufVct = handler;

	skiPosition_t cursor = skiList_begin(list);
	if(cursor == NULL)goto buf_failed;

	skiBuffer_t* newBuffer = skiBuffer_create(NULL, 0);
	if(newBuffer == NULL)goto buf_failed;

	skiBuffer_t* curBuffer = NULL;
	do{
		curBuffer = SKI_P2T(skiList_at(list, cursor), skiHandler_t);
		if(!__identifyHead(curBuffer))goto buf_failed;

		skiBuffer_append(newBuffer, curBuffer->buf, curBuffer->size);
		skiBuffer_append(newBuffer, pBufVct->buf, pBufVct->size);
	}while((cursor = skiList_next(list, cursor)));

	skiList_clear(list, _clear_buffer);
	skiList_destroy(list);
	skiBuffer_truncate(newBuffer, NULL, pBufVct->size);

	return newBuffer;
buf_failed:
	return NULL;
}

