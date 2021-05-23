#ifndef __SKI_LIST_H__
#define __SKI_LIST_H__

#include "stdlib.h"
#include "skiTypes.h"

#define skiList_construct(_type) ({ \
		_type* _ __attribute__((unused))= NULL; \
		skiList_create(sizeof(_type)); \
		})

/**
 * breif: create a list
 * Para1: size per struct data
 * return: handler of created list on success, NULL if error
 */
skiHandler_t skiList_create(size_t dataSize);

/**
 * breif: insert a node into list
 * Para1: handler of list to be operated
 * para2: position of list node, new node will be front of this one
 * para3: pointer of struct data(in)
 * return: position of inserted node on success, NULL if error
 */
skiPosition_t skiList_insert(skiHandler_t handler, skiPosition_t pos, void* data);

/**
 * breif: delete a node from list
 * Para1: handler of list to be operated
 * para2: position of list node to be deleted
 * para3: pointer of struct data(out)
 * return: next position of deleted node on success, NULL if error
 */
skiPosition_t skiList_delete(skiHandler_t handler, skiPosition_t pos, void* data);

/**
 * breif: insert a node into list's tail
 * Para1: handler of list to be operated
 * para2: pointer of struct data(in)
 * return: position of inserted node on success, NULL if error
 */
skiPosition_t skiList_pushBack(skiHandler_t handler, void* data);

/**
 * breif: insert a node into list's head
 * Para1: handler of list to be operated
 * para2: pointer of struct data(in)
 * return: position of inserted node on success, NULL if error
 */
skiPosition_t skiList_pushFront(skiHandler_t handler, void* data);

/**
 * breif: delete a node from list's tail
 * Para1: handler of list to be operated
 * para2: pointer of struct data(out)
 * return: next position of deleted node on success, NULL if error
 */
skiPosition_t skiList_popBack(skiHandler_t handler, void* data);

/**
 * breif: delete a node from list's head
 * Para1: handler of list to be operated
 * para2: pointer of struct data(out)
 * return: next position of deleted node on success, NULL if error
 */
skiPosition_t skiList_popFront(skiHandler_t handler, void* data);

/**
 * breif: search node
 * Para1: handler of list to be operated
 * para2: pointer of struct data(cmp)
 * para3: function to compare data and node in list
	searching would stop if cmpFunc return 0
 * return: position of found node in list, or NULL if not found
 */
skiPosition_t skiList_search(skiHandler_t handler, void* data, skiFunc2_t cmpFunc);

/**
 * breif: for each of list
 * Para1: handler of list to be operated
 * para2: function to deal every node in list
	traversal would be stopped when func2 return true(not 0)
	after all nodes dealed, func2 would be called with NULL in its 1st para
 * para3: func2's 2nd para
 * return: func2's last return
 */
int skiList_foreach(skiHandler_t handler, skiFunc2_t func2, void* arg);

/**
 * breif: clear list
 * Para1: handler of list to be operated
 * Para2: function for clearing node
	this node would be deleted when clearFunc is not NULL and return false(0),
	else this node would be left, and call it on next node.
 * return: void
 */
void skiList_clear(skiHandler_t handler, skiFunc1_t clearFunc);

/**
 * breif: destroy and free list
 * Para1: handler of list to be operated
 * return: void
 * ps: make sure node data has no pointer to be freed,
	or call skiList_foreach() to free them if can not
 */
void skiList_destroy(skiHandler_t handler);

/**
 * breif: get pointer of node data by position
 * Para1: handler of list to be operated
 * Para2: position of node in list
 * return: pointer of node data, NULL if error
 */
void* skiList_at(skiHandler_t handler, skiPosition_t pos);

/**
 * breif: get next position of this one
 * Para1: handler of list to be operated
 * Para2: position of specified node
 * return: next position of pos, NULL if pos is tail node
 */
skiPosition_t skiList_posNext(skiHandler_t handler, skiPosition_t pos);

/**
 * breif: get prev position of this one
 * Para1: handler of list to be operated
 * Para2: position of specified node
 * return: prev position of pos, NULL if pos is tail node
 */
skiPosition_t skiList_posPrev(skiHandler_t handler, skiPosition_t pos);

/**
 * breif: get size of list
 * Para1: handler of list to be operated
 * return: size of list, 0 if error
 */
size_t skiList_size(skiHandler_t handler);

/**
 * breif: copy a list
 * Para1: handler of list to be operated
 * return: a new list copied handler on success, NULL if error
 */
skiHandler_t skiList_copy(skiHandler_t handler);

/**
 * breif: sort list
 * Para1: handler of list to be operated
 * para2: function to compare 2 node in list
	return positive value if data1 > data2
	return negative value if data1 < data2
	return 0 value if data1 == data2
 * return: void
 */
void skiList_sort(skiHandler_t handler, skiFunc2_t cmpFunc);

/**
 * breif: insert with sorting
 * Para1: handler of list to be operated
 * Para2: pointer of data to insert
 * para3: function to compare 2 node in list
	return positive value if data1 > data2
	return negative value if data1 < data2
	return 0 value if data1 == data2
 * return: position of inserted node on success, NULL if error
 */
skiPosition_t skiList_arrange(skiHandler_t handler, void* data, skiFunc2_t cmpFunc);

/**
 * breif: reverse list
 * Para1: handler of list to be operated
 * return: void
 */
void skiList_reverse(skiHandler_t handler);
#endif
