#include "skiContainor.h"

static char skiListID;

typedef struct _pulleyNode
{
	struct _pulleyNode* prev;
	struct _pulleyNode* next;
}PulleyNode_t;

typedef struct _listHead{
	PulleyNode_t pulley;
	size_t capacity;
	size_t listSize;
	size_t dataSize;
	void* headID;
}ListHead_t;

typedef struct _listNode{
	PulleyNode_t pulley;
	//void* nodeID;
	char data[0];
}ListNode_t;

#define memset __builtin_memset
#define memcpy __builtin_memcpy

#define __identifyHead(_pHead) ((_pHead) && ((ListHead_t*)_pHead)->headID == &skiListID)
#define __identifyNode(_pNode) ((_pNode) && ((PulleyNode_t*)_pNode)->prev && ((PulleyNode_t*)_pNode)->next)


#define __swapNode(_pNode1, _pNode2) do{ \
	__swapPos(_pNode1, _pNode2); \
	__swapValue(_pNode1, _pNode2); \
}while(0)

#define __swapValue(_val1, _val2) do{ \
	typeof(_val1) _tmp = (_val1); \
	(_val1) = (_val2); \
	(_val2) = _tmp; \
}while(0)

static void inline __initNode(PulleyNode_t* node)
{
	node->prev = node->next = node;
}

static void inline __insertNode(PulleyNode_t* front, PulleyNode_t* node, PulleyNode_t* back)
{
	front->next = node;
	back->prev = node;
	node->prev = front;
	node->next = back;
}

static void inline __insertList(PulleyNode_t* front, PulleyNode_t* start, PulleyNode_t* end, PulleyNode_t* back)
{
	front->next = start;
	back->prev = end;
	start->prev = front;
	end->next = back;
}

static void inline __deleteNode(PulleyNode_t* front, PulleyNode_t* back)
{
	front->next = back;
	back->prev = front;
}

static void inline __rotatePrev(PulleyNode_t* node)
{
	__deleteNode(node->prev, node->next);
	__insertNode(node->prev->prev, node, node->prev);
}

static void inline __rotateNext(PulleyNode_t* node)
{
	__deleteNode(node->prev, node->next);
	__insertNode(node->next, node, node->next->next);
}

static void inline __swapPos(PulleyNode_t* node1, PulleyNode_t* node2)
{
	if(node1 == node2)return;
	else if(node1->next == node2)__rotateNext(node1);
	else if(node1->prev == node2)__rotatePrev(node1);
	else{
		__swapValue(*node1, *node2);
		node1->prev->next = node1;
		node1->next->prev = node1;
		node2->prev->next = node2;
		node2->next->prev = node2;
	}
}

static int inline __is_empty(PulleyNode_t* head)
{
	return head->next == head;
}

skiHandler_t skiList_create(size_t dataSize, size_t capacity)
{
	ListHead_t* pListHead = malloc(sizeof(ListHead_t));
	if(pListHead == NULL)goto malloc_failed;

	pListHead->dataSize = dataSize;
	pListHead->capacity = capacity;
	pListHead->listSize = 0;
	pListHead->headID = &skiListID;
	__initNode(&pListHead->pulley);

malloc_failed:
	return pListHead;
}

size_t skiList_insert(skiHandler_t handler, skiPosition_t pos, void* data)
{
	if(!__identifyHead(handler))goto list_failed;
	ListHead_t* pListHead = handler;

	if(pListHead->listSize == pListHead->capacity)goto list_failed;

	if(!__identifyNode(pos))pos = ((PulleyNode_t*)handler)->next;
	PulleyNode_t* pEntry = pos;

	ListNode_t* pListNode = malloc(sizeof(ListNode_t) + pListHead->dataSize);
	if(pListNode == NULL)goto list_failed;

	__insertNode(pEntry->prev, &pListNode->pulley, pEntry);

	if(data)memcpy(pListNode->data, data, pListHead->dataSize);
	//pListNode->nodeID = handler;

	return pListHead->listSize += 1;
list_failed:
	return 0;
}

size_t skiList_delete(skiHandler_t handler, skiPosition_t pos, void* data)
{
	if(!__identifyHead(handler))goto list_failed;
	if(!__identifyNode(pos))goto list_failed;
	if(handler == pos)goto list_failed;
	ListHead_t* pListHead = handler;
	PulleyNode_t* pNode = pos;
	ListNode_t* pListNode = pos;

	if(pListHead->listSize == 0)goto list_failed;

	pos = pNode->next;
	__deleteNode(pNode->prev, pNode->next);
	__initNode(pNode);

	if(data)memcpy(data, pListNode->data, pListHead->dataSize);
	//pListNode->nodeID = NULL;
	free(pNode);
	return pListHead->listSize--;

list_failed:
	return 0;
}

size_t skiList_pushBack(skiHandler_t handler, void* data)
{
	if(!__identifyHead(handler))return 0;
	return skiList_insert(handler, ((PulleyNode_t*)handler), data);
}

size_t skiList_pushFront(skiHandler_t handler, void* data)
{
	if(!__identifyHead(handler))return 0;
	return skiList_insert(handler, ((PulleyNode_t*)handler)->next, data);
}

size_t skiList_popBack(skiHandler_t handler, void* data)
{
	if(!__identifyHead(handler))return 0;
	return skiList_delete(handler, ((PulleyNode_t*)handler)->prev, data);
}

size_t skiList_popFront(skiHandler_t handler, void* data)
{
	if(!__identifyHead(handler))return 0;
	return skiList_delete(handler, ((PulleyNode_t*)handler)->next, data);
}

skiPosition_t skiList_search(skiHandler_t handler, void* data, skiFunc2_t cmpFunc)
{
	if(!__identifyHead(handler))goto list_failed;
	//ListHead_t* pListHead = handler;
	PulleyNode_t* pHead = handler, *cursor = NULL;

	for(cursor = pHead->next; cursor != pHead; cursor = cursor->next){
		if(0 == cmpFunc(((ListNode_t*)cursor)->data, data))break;
	}

	return cursor;

list_failed:
	return NULL;
}

int skiList_foreach(skiHandler_t handler, skiFunc2_t func2, void* arg)
{
	if(!__identifyHead(handler))goto list_failed;
	if(func2 == NULL)goto list_failed;
	//ListHead_t* pListHead = handler;
	PulleyNode_t* pHead = handler, *cursor = NULL;

	for(cursor = pHead->next; cursor != pHead; cursor = cursor->next){
		func2(((ListNode_t*)cursor)->data, arg);
	}

	return func2(NULL, arg);
list_failed:
	return 0;
}

void skiList_clear(skiHandler_t handler)
{
	if(!__identifyHead(handler))return;
	ListHead_t* pListHead = handler;
	PulleyNode_t* pHead = handler;
	ListNode_t* pListNode = NULL;

	while(pHead->next != pHead){
		pListNode = (ListNode_t*)pHead->next;
		__deleteNode(pListNode->pulley.prev, pListNode->pulley.next);
		__initNode(&pListNode->pulley);
		//pListNode->nodeID = NULL;
		free(pListNode);
	}
	pListNode = NULL;

	pListHead->listSize = 0;
}

void skiList_destroy(skiHandler_t handler)
{
	if(!__identifyHead(handler))return;
	ListHead_t* pListHead = handler;

	skiList_clear(handler);
	pListHead->headID = NULL;
	free(handler);
}

void* skiList_at(skiPosition_t pos)
{
	if(!__identifyNode(pos))goto list_failed;

	return ((ListNode_t*)pos)->data;
list_failed:
	return NULL;
}

skiPosition_t skiList_posNext(skiHandler_t handler, skiPosition_t pos)
{
	if(!__identifyHead(handler))goto list_failed;
	if(!__identifyNode(pos))goto list_failed;

	pos = ((PulleyNode_t*)pos)->next;
	if(pos == handler)goto list_failed;

	return pos;
list_failed:
	return NULL;
}

skiPosition_t skiList_posPrev(skiHandler_t handler, skiPosition_t pos)
{
	if(!__identifyHead(handler))goto list_failed;
	if(!__identifyNode(pos))goto list_failed;

	pos = ((PulleyNode_t*)pos)->prev;
	if(pos == handler)goto list_failed;

	return pos;
list_failed:
	return NULL;
}

size_t skiList_size(skiHandler_t handler)
{
	if(!__identifyHead(handler))return 0;
	return ((ListHead_t*)handler)->listSize;
}

void skiList_reCap(skiHandler_t handler, size_t capacity)
{
	if(!__identifyHead(handler))return;
	ListHead_t* pListHead = handler;

	if(pListHead->listSize > capacity)return;

	pListHead->capacity = capacity;
}

static int __copy_node(void* data, void* handler)
{
	if(data)skiList_pushBack(handler, data);
	return 0;
}

skiHandler_t skiList_copy(skiHandler_t handler)
{
	if(!__identifyHead(handler))goto list_failed;
	ListHead_t* pListHead = handler;

	ListHead_t* pListHeadNew = skiList_create(pListHead->dataSize, pListHead->capacity);
	if(pListHeadNew == NULL)goto list_failed;

	skiList_foreach(pListHead, __copy_node, pListHeadNew);

	return pListHeadNew;
list_failed:
	return NULL;
}

size_t skiList_join(skiHandler_t handler1, skiHandler_t handler2)
{
	if(!__identifyHead(handler1))goto list_failed;
	if(!__identifyHead(handler2))goto list_failed;
	PulleyNode_t* pHead1 = handler1;
	PulleyNode_t* pHead2 = handler2;
	ListHead_t* pListHead1 = handler1;
	ListHead_t* pListHead2 = handler2;

	if(pListHead1->dataSize != pListHead2->dataSize)goto list_failed;
	
	if(pListHead2->listSize)__insertList(pHead1->prev, pHead2->next, pHead2->prev, pHead1);
	__initNode(pHead2);

	pListHead1->listSize += pListHead2->listSize;
	if(pListHead1->capacity < pListHead1->listSize)pListHead1->capacity = pListHead1->listSize;

	skiList_destroy(handler2);

	return pListHead1->listSize;
list_failed:
	return 0;
}

skiHandler_t skiList_cut(skiHandler_t handler, skiPosition_t pos, size_t length)
{
	if(!__identifyHead(handler))goto list_failed;
	if(!__identifyNode(pos))goto list_failed;
	if(handler == pos)goto list_failed;
	size_t cnt = 1;
	PulleyNode_t* cursor = pos, *start = pos;
	ListHead_t* pListHead = handler;

	while(cnt < length && cursor->next != &pListHead->pulley){
		if(++cnt > pListHead->listSize)goto list_failed;
		cursor = cursor->next;
	}

	ListHead_t* pListHeadNew = skiList_create(pListHead->dataSize, pListHead->capacity);
	if(pListHeadNew == NULL)goto list_failed;

	__deleteNode(start->prev, cursor->next);
	__insertList(&pListHeadNew->pulley, start, cursor, &pListHeadNew->pulley);

	pListHead->listSize -= cnt;
	pListHeadNew->listSize = cnt;

	return pListHeadNew;
list_failed:
	return NULL;
}

#if 0 //unused but mark here
static void __quick_sort(PulleyNode_t* start, PulleyNode_t* end, skiFunc2_t cmpFunc)
{
	if(start->next == end || start == end)return;
	PulleyNode_t* front = start, *back = end->prev;

	while(1){
		while(front != back && cmpFunc(((ListNode_t*)start)->data, ((ListNode_t*)back)->data) <= 0)
			back = back->prev;

		while(front != back && cmpFunc(((ListNode_t*)start)->data, ((ListNode_t*)front)->data) >= 0)
			front = front->next;

		if(front == back)break;

		__swapNode(front, back);
	}

	__swapNode(start, front);

	__quick_sort(start, front->next, cmpFunc);
	__quick_sort(front->next, end, cmpFunc);
}
#endif

static void __insert_sort(PulleyNode_t* pHead, skiFunc2_t cmpFunc)
{
	PulleyNode_t* sorted = NULL;
	PulleyNode_t* cursor = NULL;
	PulleyNode_t* anchor = NULL;
	for(sorted = pHead->next; sorted->next != pHead;){
		for(anchor = cursor = sorted->next; cursor->prev != pHead; cursor = cursor->prev)
			if(cmpFunc(((ListNode_t*)cursor->prev)->data, ((ListNode_t*)anchor)->data) < 0)
				break;

		if(cursor == sorted->next)sorted = cursor;
		else{
			__deleteNode(anchor->prev, anchor->next);
			__insertNode(cursor->prev, anchor, cursor);
		}
	}
}

static void __merge_sort(PulleyNode_t* pHead, size_t size, skiFunc2_t cmpFunc)
{
	if(size <= 1)return;
	if(size <= 11)return __insert_sort(pHead, cmpFunc);
	size_t subSize = size / 2;
	PulleyNode_t tHead = {&tHead, &tHead};
	PulleyNode_t* tCursor = pHead->prev;
	PulleyNode_t* mCursor = pHead->next;
	
	//find middle pointer
	while(subSize--)mCursor = mCursor->next;

	//cut list into 2
	__deleteNode(mCursor->prev, tCursor->next);
	__insertList(&tHead, mCursor, tCursor, &tHead);

	//sort each sub list
	__merge_sort(pHead, size/2, cmpFunc);
	__merge_sort(&tHead, size/2 + (size&1), cmpFunc);

	//merge 2 sorted lists into 1
	mCursor = pHead->next;
	while(!__is_empty(&tHead)){
		tCursor = tHead.next;

		while(mCursor != pHead && cmpFunc(((ListNode_t*)mCursor)->data, ((ListNode_t*)tCursor)->data) <= 0)
			mCursor = mCursor->next;

		if(mCursor == pHead){
			__insertList(mCursor->prev, tHead.next, tHead.prev, mCursor);
			__initNode(&tHead);
			break;
		}
		
		while(tCursor != &tHead && cmpFunc(((ListNode_t*)tCursor)->data, ((ListNode_t*)mCursor)->data) < 0)
			tCursor = tCursor->next;

		__insertList(mCursor->prev, tHead.next, tCursor->prev, mCursor);
		__deleteNode(&tHead, tCursor);
	}
}

void skiList_sort_st(skiHandler_t handler, skiFunc2_t cmpFunc)
{
	if(!__identifyHead(handler) || cmpFunc == NULL)return;
	__insert_sort(handler, cmpFunc);
}

void skiList_sort(skiHandler_t handler, skiFunc2_t cmpFunc)
{
	if(!__identifyHead(handler) || cmpFunc == NULL)return;
	//__quick_sort(((PulleyNode_t*)handler)->next, ((PulleyNode_t*)handler), cmpFunc);
	__merge_sort(handler, ((ListHead_t*)handler)->listSize, cmpFunc);
}

void skiList_reverse(skiHandler_t handler)
{
	if(handler == NULL)return;
	PulleyNode_t* pHead = handler;
	PulleyNode_t* front = pHead->next, *back = pHead->prev;
	ListHead_t* pListHead = handler;

	if(pListHead->listSize & 1){
		while(front != back){
			__swapNode(front, back);
			front = front->next;
			back = back->prev;
		}
	}else{
		while(front->prev != back){
			__swapNode(front, back);
			front = front->next;
			back = back->prev;
		}
	}
}

