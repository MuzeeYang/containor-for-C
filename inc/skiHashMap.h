#ifndef __SKI_HASHMAP_H__
#define __SKI_HASHMAP_H__

#include "stdlib.h"
#include "skiTypes.h"

#define skiHashMap_construct(_type, _cap) ({ \
		_type* _ __attribute__((unused))= NULL; \
		skiHashMap_create(sizeof(_type), (_cap)); \
		})

/**
 * breif: create a hash map
 * Para1: size per struct data
 * Para2: new map's capacity, but it would be increased automatically
 * return: handler of new map
 */
skiHandler_t skiHashMap_create(size_t dataSize, size_t capacity);

/**
 * brief: push a node into map
 * Para1: handler of map to be operated
 * para2: key of new node
 * para3: pointer of value of new node
 * return: size of map after pushing on success, 0 if error
 * ps: pointer value would be replaced with old node, if key's already existed
 */
size_t skiHashMap_push(skiHandler_t handler, char* key, void* value);

/**
 * breif: pop a node from hash map
 * Para1: handler of map to be operated
 * para2: key of node to be popped
 * para3: pointer of value of node to be popped
	if value is not NULL, value would be set with deleted node
 * return: size of map before popping on success, 0 if error
 */
size_t skiHashMap_pop(skiHandler_t handler, char* key, void* value);

/**
 * breif: get value by key
 * Para1: handler of map to be operated
 * para2: key of specified node
 * return: pointer of node on success, NULL if error
 */
void* skiHashMap_at(skiHandler_t handler, char* key);

/**
 * breif: for each on hash map
 * Para1: handler of map to be operated
 * para2: function to deal every node
	traversal would be stopped when func3 return true(not 0)
	after all nodes dealed, func3 would be called with NULL in its 2nd para
 * para3: func3's 3rd para
 * return: func3's last return
 */
int skiHashMap_foreach(skiHandler_t handler, skiFunc3_t func3, void* arg);

/**
 * breif: get size of hash map
 * Para1: handler of map to be operated
 * return: size of map on success, 0 if error
 */
size_t skiHashMap_size(skiHandler_t handler);

/**
 * breif: clear hash map
 * Para1: handler of map to be operated
 * Para2: function for clearing node
	this node would be deleted when clearFunc is not NULL and return false(0),
	else this node would be left, and call it on next node.
 * return: void
 */
void skiHashMap_clear(skiHandler_t handler, skiFunc2_t clearFunc);

/**
 * breif: destory hash map
 * Para1: handler of map to be operated
 * return: void
 * ps: make sure node data has no pointer to be freed,
	or call skiHashMap_foreach() to free them if can not
 */
void skiHashMap_destroy(skiHandler_t handler);

#endif
