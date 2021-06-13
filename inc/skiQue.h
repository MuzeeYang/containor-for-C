#ifndef __SKI_QUE_H__
#define __SKI_QUE_H__

#include "stdlib.h"
#include "skiTypes.h"

typedef int (*skiFuncPause_t)(size_t*, void*);

#define skiQue_construct(_type, _cap) ({ \
		_type* _ __attribute__((unused))= NULL; \
		unsigned char _pow = 0; \
		while((_cap) > (1 << _pow))_pow++; \
		skiQue_create(sizeof(_type), _pow); \
		})

/**
 * breif: create a que
 * Para1: size of each node
 * Para2: power of 2, deciding capacity of que
 * return: handler of que on success, NULL if error
 */
skiHandler_t skiQue_create(size_t dataSize, unsigned char power);

/**
 * breif: destroy a que
 * Para1: handler of que to be operated
 * return: void
 */
void skiQue_destroy(skiHandler_t handler);

/**
 * breif: clear a que
 * Para1: handler of que to be operated
 * return: void
 */
void skiQue_clear(skiHandler_t handler);

/**
 * breif: push a node into que
 * Para1: handler of que to be operated
 * Para2: pointer of node data to push
 * Para3: function for polling when queue was full, if NULL return
 * Para4: arg of pauseFunc
 * return: size of que has been pushed on success, 0 if error
 */
size_t skiQue_push(skiHandler_t handler, void* data, skiFuncPause_t pauseFunc, void* arg);

/**
 * breif: pop a node from que
 * Para1: handler of que to be operated
 * Para2: pointer of node data to pop
 * Para3: function for polling whem queue was empty, if NULL return
 * Para4: arg of pauseFunc
 * return: size of que has not been popped on success, 0 if error
 */
size_t skiQue_pop(skiHandler_t handler, void* data, skiFuncPause_t pauseFunc, void* arg);

/**
 * breif: get size of queue
 * Para1: handler of que to be operated
 * return: size of queue on success, 0 if error
 */
size_t skiQue_size(skiHandler_t handler);

#endif
