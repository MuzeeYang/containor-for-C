#ifndef __SKICONTAINOR_H__
#define __SKICONTAINOR_H__

#include "stdlib.h"

typedef void* skiHandler_t;
typedef void* skiPosition_t;
//typedef void* skiLinker_t;
typedef size_t skiIndex_t; 
typedef unsigned int skiHashCode_t;
typedef unsigned int skiU32_t;
typedef int (*skiFunc1_t)(void*);
typedef int (*skiFunc2_t)(void*, void*);
typedef int (*skiFunc3_t)(void*, void*, void*);
typedef int (*skiFuncIPP_t)(skiIndex_t, void*, void*);

//================== LIST ======================
#define SKILIST_NOLIMIT		(-1)

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
 * Para1: position of node in list
 * return: pointer of node data, NULL if error
 */
void* skiList_at(skiPosition_t pos);

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
 * breif: join 2 list2 into 1
 * Para1: handler of list to be operated
 * para2: handler of 2nd list to be joined into 1st one
 * return: size of joined list(handler1)
 * ps: handler2 append into handler1, handler2 would be destroyed
 */
size_t skiList_join(skiHandler_t handler1, skiHandler_t handler2);

/**
 * breif: cut 1 list into 2
 * Para1: handler of list to be operated
 * para2: position of node to cut on start
 * para3: length to cut
 * return: handler of new list with cuted nodes
 */
skiHandler_t skiList_cut(skiHandler_t handler, skiPosition_t pos, size_t length);

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


//=================== MAP =========================
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
 * return: size of que has been pushed on success, 0 if error
 */
size_t skiQue_push(skiHandler_t handler, void* data);

/**
 * breif: pop a node from que
 * Para1: handler of que to be operated
 * Para2: pointer of node data to pop
 * return: size of que has not been popped on success, 0 if error
 */
size_t skiQue_pop(skiHandler_t handler, void* data);

/**
 * breif: poll to wait
 * Para1: handler of que to be operated
 * Para2: flags to action
	SKIQUE_BLOCK_READ -> poll popping
	SKIQUE_BLOCK_WRITE -> poll pushing
	SKIQUE_NON_BLOCK -> no block whether pop or push
 * return: size of que currently
 */
size_t skiQue_poll(skiHandler_t handler, char rw);


//================== BUFFER ========================
#define SKIBUFFER_IDX_END	(-1)

/**
 * breif: create a buffer
 * Para1: pointer of a buf to init, if NULL init buffer with 0
 * Para2: length of buf, or size of buffer to create if para1 is NULL.
	if length less than 16, length would be set to 16.
 * return: handler of buffer
 */
skiHandler_t skiBuffer_create(void* buf, size_t length);

/**
 * breif: destroy a buffer
 * Para1: handler of buffer to be operated
 * return: void
 */
void skiBuffer_destroy(skiHandler_t handler);

/**
 * breif: copy a buffer
 * Para1: handler of buffer to be operated
 * return: handler of new a buffer
 */
skiHandler_t skiBuffer_copy(skiHandler_t handler);

/**
 * breif: resize a buffer
 * Para1: handler of buffer to be operated
 * Para2: new size of buffer
 * return: size of buffer currently on success, 0 if error
 * ps: this function could increase buffer or truncate it with setting 0
 */
size_t skiBuffer_resize(skiHandler_t handler, size_t size);

/**
 * breif: insert a buf into buffer on any position
 * Para1: handler of buffer to be operated
 * Para2: position to insert. if offset is larger than size, it would be set to size.
 * Para3: pointer of buf
 * Para4: length of buf
 * return: inserted size on success, 0 if error
 * ps: offset could be size of buffer, that means insert buf append buffer
	SKIBUFFER_IDX_END would help.
 */
size_t skiBuffer_insert(skiHandler_t handler, skiIndex_t offset, void* buf, size_t length);

/**
 * breif: insert a buf into buffer on tail
 * Para1: handler of buffer to be operated
 * Para2: pointer of buf
 * Para3: length of buf
 * return: inserted size on success, 0 if error
 * ps: this function is like a special skiBuffer_insert()
 */
size_t skiBuffer_append(skiHandler_t handler, void* buf, size_t length);

/**
 * breif: cut a buf from buffer on any position
 * Para1: handler of buffer to be operated
 * Para2: position to start cutting. if offset is larger than size, it would be set to size.
 * Para3: pointer of buf(out), data cutted would be dropped if it was NULL.
 * Para4: length of buf to cut
 * return: cutted size on success, 0 if error
 * ps: cutted length might be less than para4, it would be dicided on the rest of buffer
 */
size_t skiBuffer_cut(skiHandler_t handler, skiIndex_t offset, void* buf, size_t length);

/**
 * breif: cut a buf from buffer on tail
 * Para1: handler of buffer to be operated
 * Para2: pointer of buf(out), data cutted would be dropped if it was NULL.
 * Para3: length of buf to cut
 * return: cutted size on success, 0 if error
 * ps: this function is like a special skiBuffer_cut()
 */
size_t skiBuffer_truncate(skiHandler_t handler, void* buf, size_t length);

/**
 * breif: insert a buffer into buffer on any position
 * Para1: handler of buffer to be operated
 * Para2: position to insert. if offset is larger than size, it would be set to size.
 * Para3: handler of buffer to insert
 * return: inserted size on success, 0 if error
 * ps: handler in para2 would be destroyed.
	this function is like skiBuffer_insert() with buffer instead.
 */
size_t skiBuffer_join(skiHandler_t handler1, skiIndex_t offset, skiHandler_t handler2);

/**
 * breif: cut a buffer from buffer on any position
 * Para1: handler of buffer to be operated
 * Para2: position to start cutting. if offset is larger than size, it would be set to size.
 * Para3: length of buf to cut
 * return: handler of a new buffer on success, NULL if error
 * ps: cutted length might be less than para3, it would be dicided on the rest of buffer.
	this function is like skiBuffer_cut() with buffer instead.
 */
skiHandler_t skiBuffer_slice(skiHandler_t handler, skiIndex_t offset, size_t length);

/**
 * breif: find whether buffer has buf
 * Para1: handler of buffer to be operated
 * Para2: position to start cutting. if offset is larger than size, it would be set to size.
 * Para3: pointer of buf to search
 * Para4: length of buf
 * return: index of buffer on success, size of buffer if error
 * ps: index must be less than size, this is a difference against offset
 */
skiIndex_t skiBuffer_find(skiHandler_t handler, skiIndex_t offset, void* buf, size_t length);

/**
 * breif: get data of buffer by index
 * Para1: handler of buffer to be operated
 * Para2: index of buffer, must be less than size
 * return: pointer of data in buffer on success, NULL if error
 */
char* skiBuffer_at(skiHandler_t handler, skiIndex_t idx);

/**
 * breif: get size of buffer
 * Para1: handler of buffer to be operated
 * return: size of buffer on success, 0 if error
 */
size_t skiBuffer_size(skiHandler_t handler);

//================== HASHMAP ========================

skiHandler_t skiHashMap_create(size_t dataSize, size_t capacity);
size_t skiHashMap_push(skiHandler_t handler, char* key, void* value);
size_t skiHashMap_pop(skiHandler_t handler, char* key, void* value);
void* skiHashMap_at(skiHandler_t handler, char* key);
int skiHashMap_foreach(skiHandler_t handler, skiFunc3_t func3, void* arg);
size_t skiHashMap_size(skiHandler_t handler);
void skiHashMap_clear(skiHandler_t handler, skiFunc2_t clearFunc);
void skiHashMap_destroy(skiHandler_t handler);

#endif
