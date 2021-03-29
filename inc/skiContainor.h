#ifndef __SKICONTAINOR_H__
#define __SKICONTAINOR_H__

#include "stdlib.h"

typedef void* skiHandler_t;
typedef void* skiPosition_t;
typedef int (*skiFunc1_t)(void*);
typedef int (*skiFunc2_t)(void*, void*);

#define SKILIST_NOLIMIT		(-1)

#define skiList_construct(_type) ({ \
		_type* _ __attribute__((unused))= NULL; \
		skiList_create(sizeof(_type), SKILIST_NOLIMIT); \
		})

skiHandler_t skiList_create(size_t dataSize, size_t capacity);
void* skiList_insert(skiHandler_t handler, skiPosition_t pos, void* data);
void* skiList_delete(skiHandler_t handler, skiPosition_t pos, void* data);
void* skiList_pushBack(skiHandler_t handler, void* data);
void* skiList_pushFront(skiHandler_t handler, void* data);
void* skiList_popBack(skiHandler_t handler, void* data);
void* skiList_popFront(skiHandler_t handler, void* data);
skiPosition_t skiList_search(skiHandler_t handler, void* data, skiFunc2_t cmpFunc);
int32_t skiList_foreach(skiHandler_t handler, skiFunc2_t func2, void* arg);
void skiList_clear(skiHandler_t handler, skiFunc1_t freeFunc);
void skiList_destroy(skiHandler_t handler, skiFunc1_t freeFunc);
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


#endif
