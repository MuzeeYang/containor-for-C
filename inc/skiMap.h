#ifndef __SKI_MAP_H__
#define __SKI_MAP_H__

#include "stdlib.h"
#include "skiTypes.h"

#define skiMap_construct(_type) ({ \
		_type* _ __attribute__((unused))= NULL; \
		skiMap_create(sizeof(_type)); \
		})

/**
 * breif: create a map
 * Para1: size per struct data
 * return: handler of new map
 */
skiHandler_t skiMap_create(size_t dataSize);

/**
 * brief: push a node into map
 * Para1: handler of map to be operated
 * para2: key of new node
 * para3: pointer of value of new node
 * return: size of map after pushing on success, 0 if error
 * ps: pointer value would be replaced with old node, if key's already existed
 */
size_t skiMap_push(skiHandler_t handler, skiIndex_t key, void* value);

/**
 * breif: pop a node from map
 * Para1: handler of map to be operated
 * para2: key of node to be popped
 * para3: pointer of value of node to be popped
	if value is not NULL, value would be set with deleted node
 * return: size of map before popping on success, 0 if error
 */
size_t skiMap_pop(skiHandler_t handler, skiIndex_t key, void* value);

/**
 * breif: get value by key
 * Para1: handler of map to be operated
 * para2: key of specified node
 * return: pointer of node on success, NULL if error
 */
void* skiMap_at(skiHandler_t handler, skiIndex_t key);

/**
 * breif: for each on map
 * Para1: handler of map to be operated
 * para2: function to deal every node
	traversal would be stopped when funcIPP return true(not 0)
	after all nodes dealed, funcIPP would be called with NULL in its 2nd para
 * para3: funcIPP's 3rd para
 * return: funcIPP's last return
 */
int skiMap_foreach(skiHandler_t handler, skiFuncIPP_t funcIPP, void* arg);

/**
 * breif: clear map
 * Para1: handler of map to be operated
 * return: void
 * ps: make sure node data has no pointer to be freed,
	or call skiMap_foreach() to free them if can not
 */
void skiMap_clear(skiHandler_t handler);

/**
 * breif: destory map
 * Para1: handler of map to be operated
 * return: void
 * ps: make sure node data has no pointer to be freed,
	or call skiMap_foreach() to free them if can not
 */
void skiMap_destroy(skiHandler_t handler);

/**
 * breif: get size of map
 * Para1: handler of map to be operated
 * return: size of map on success, 0 if error
 */
size_t skiMap_size(skiHandler_t handler);

/**
 * breif: get the minimum node
 * Para1: handler of map to be operated
 * para2: pointer of key to get minimum key
 * return: pointer to the value of minimum node
 */
void* skiMap_min(skiHandler_t handler, skiIndex_t* outIdx);

/**
 * breif: get the maximum node
 * Para1: handler of map to be operated
 * para2: pointer of key to get maximum key
 * return: pointer to the value of maximum node
 */
void* skiMap_max(skiHandler_t handler, skiIndex_t* outIdx);

#endif
