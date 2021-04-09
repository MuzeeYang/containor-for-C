#ifndef __SKICONTAINOR_H__
#define __SKICONTAINOR_H__

#include "stdlib.h"

typedef void* skiHandler_t;
typedef void* skiPosition_t;
typedef size_t skiIndex_t; 
typedef int (*skiFunc1_t)(void*);
typedef int (*skiFunc2_t)(void*, void*);
typedef int (*skiFunc3_t)(skiIndex_t, void*, void*);

//================== LIST ======================
#define SKILIST_NOLIMIT		(-1)

#define skiList_construct(_type) ({ \
		_type* _ __attribute__((unused))= NULL; \
		skiList_create(sizeof(_type), SKILIST_NOLIMIT); \
		})

skiHandler_t skiList_create(size_t dataSize, size_t capacity);
size_t skiList_insert(skiHandler_t handler, skiPosition_t pos, void* data);
size_t skiList_delete(skiHandler_t handler, skiPosition_t pos, void* data);
size_t skiList_pushBack(skiHandler_t handler, void* data);
size_t skiList_pushFront(skiHandler_t handler, void* data);
size_t skiList_popBack(skiHandler_t handler, void* data);
size_t skiList_popFront(skiHandler_t handler, void* data);
skiPosition_t skiList_search(skiHandler_t handler, void* data, skiFunc2_t cmpFunc);
int skiList_foreach(skiHandler_t handler, skiFunc2_t func2, void* arg);
void skiList_clear(skiHandler_t handler);
void skiList_destroy(skiHandler_t handler);
void* skiList_at(skiPosition_t pos);
skiPosition_t skiList_posNext(skiHandler_t handler, skiPosition_t pos);
skiPosition_t skiList_posPrev(skiHandler_t handler, skiPosition_t pos);
size_t skiList_size(skiHandler_t handler);
void skiList_reCap(skiHandler_t handler, size_t capacity);
skiHandler_t skiList_copy(skiHandler_t handler);
size_t skiList_join(skiHandler_t handler1, skiHandler_t handler2);
skiHandler_t skiList_cut(skiHandler_t handler, skiPosition_t pos, size_t length);
void skiList_sort(skiHandler_t handler, skiFunc2_t cmpFunc);
void skiList_reverse(skiHandler_t handler);


//=================== MAP =========================
#define skiMap_construct(_type) ({ \
		_type* _ __attribute__((unused))= NULL; \
		skiMap_create(sizeof(_type)); \
		})


skiHandler_t skiMap_create(size_t dataSize);
size_t skiMap_push(skiHandler_t handler, skiIndex_t key, void* value);
size_t skiMap_pop(skiHandler_t handler, skiIndex_t key, void* value);
void* skiMap_at(skiHandler_t handler, skiIndex_t key);
void skiMap_foreach(skiHandler_t handler, skiFunc3_t func3, void* arg);
void skiMap_clear(skiHandler_t handler);
void skiMap_destroy(skiHandler_t handler);
size_t skiMap_size(skiHandler_t handler);
skiIndex_t skiMap_min(skiHandler_t handler);
skiIndex_t skiMap_max(skiHandler_t handler);


//================== QUE =========================
#define skiQue_construct(_type, _cap) ({ \
		_type* _ __attribute__((unused))= NULL; \
		unsigned char _pow = 0; \
		while((_cap) > (1 << _pow))_pow++; \
		skiQue_create(sizeof(_type), _pow); \
		})

#define SKIQUE_BLOCK_READ	(0)
#define SKIQUE_BLOCK_WRITE	(1)
#define SKIQUE_NON_BLOCK	(0xb0)

skiHandler_t skiQue_create(size_t dataSize, unsigned char power);
void skiQue_destroy(skiHandler_t handler);
void skiQue_clear(skiHandler_t handler);
size_t skiQue_push(skiHandler_t handler, void* data);
size_t skiQue_pop(skiHandler_t handler, void* data);
int skiQue_poll(skiHandler_t handler, char rw);


//================== BUFFER ========================
#define SKIBUFFER_IDX_END	(-1)

skiHandler_t skiBuffer_create(void* buf, size_t length);
void skiBuffer_destroy(skiHandler_t handler);
skiHandler_t skiBuffer_copy(skiHandler_t handler);
size_t skiBuffer_reSize(skiHandler_t handler, size_t size);
size_t skiBuffer_insert(skiHandler_t handler, skiIndex_t offset, void* buf, size_t length);
size_t skiBuffer_depend(skiHandler_t handler, void* buf, size_t length);
size_t skiBuffer_cut(skiHandler_t handler, skiIndex_t offset, void* buf, size_t length);
size_t skiBuffer_truncate(skiHandler_t handler, void* buf, size_t length);
size_t skiBuffer_join(skiHandler_t handler1, skiIndex_t offset, skiHandler_t handler2);
skiHandler_t skiBuffer_slice(skiHandler_t handler, skiIndex_t offset, size_t length);
skiIndex_t skiBuffer_find(skiHandler_t handler, skiIndex_t offset, void* buf, size_t length);
char* skiBuffer_at(skiHandler_t handler, skiIndex_t idx);
size_t skiBuffer_size(skiHandler_t handler);

#endif
