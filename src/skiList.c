#include "skiList.h"

static char skiListID;

typedef struct _pulleyNode
{
	struct _pulleyNode* prev;
	struct _pulleyNode* next;
}PulleyNode_t;

typedef struct _listHead{
	PulleyNode_t pulley;
	size_t listSize;
	size_t dataSize;
	void* headID;
}ListHead_t;

typedef struct _listNode{
	PulleyNode_t pulley;
	void* nodeID;
	char data[0];
}ListNode_t;

#define __identifyHead(_pHead) ((_pHead) && ((ListHead_t*)_pHead)->headID == &skiListID)
#define __identifyNode(_pHead, _pNode) ((_pNode) && (_pNode) != (_pHead) && ((ListNode_t*)(_pNode))->nodeID == (_pHead))


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

skiHandler_t skiList_create(size_t dataSize)
{
	ListHead_t* pListHead = malloc(sizeof(ListHead_t));
	if(pListHead == NULL)goto malloc_failed;

	pListHead->dataSize = dataSize;
	pListHead->listSize = 0;
	pListHead->headID = &skiListID;
	__initNode(&pListHead->pulley);

malloc_failed:
	return pListHead;
}

skiPosition_t skiList_insert(skiHandler_t handler, skiPosition_t pos, void* data)
{
	if(!__identifyHead(handler))goto list_failed;
	ListHead_t* pListHead = handler;

	if(handler != pos && !__identifyNode(handler, pos))goto list_failed;
	PulleyNode_t* pEntry = pos;

	ListNode_t* pListNode = malloc(sizeof(ListNode_t) + pListHead->dataSize);
	if(pListNode == NULL)goto list_failed;

	__insertNode(pEntry->prev, &pListNode->pulley, pEntry);

	if(data)memcpy(pListNode->data, data, pListHead->dataSize);
	pListNode->nodeID = handler;
	pListHead->listSize++;

	return (skiPosition_t)pListNode;
list_failed:
	return NULL;
}

skiPosition_t skiList_delete(skiHandler_t handler, skiPosition_t pos, void* data)
{
	if(!__identifyHead(handler))goto list_failed;
	if(!__identifyNode(handler, pos))goto list_failed;
	ListHead_t* pListHead = handler;
	PulleyNode_t* pNode = pos;
	ListNode_t* pListNode = pos;

	if(pListHead->listSize == 0)goto list_failed;

	pos = pNode->next;
	__deleteNode(pNode->prev, pNode->next);
	__initNode(pNode);

	if(data)memcpy(data, pListNode->data, pListHead->dataSize);
	pListNode->nodeID = NULL;
	free(pNode);
	pListHead->listSize--;

	return (skiPosition_t)pos;
list_failed:
	return NULL;
}

skiPosition_t skiList_pushBack(skiHandler_t handler, void* data)
{
	if(!__identifyHead(handler))return NULL;
	return skiList_insert(handler, ((PulleyNode_t*)handler), data);
}

skiPosition_t skiList_pushFront(skiHandler_t handler, void* data)
{
	if(!__identifyHead(handler))return NULL;
	return skiList_insert(handler, ((PulleyNode_t*)handler)->next, data);
}

skiPosition_t skiList_popBack(skiHandler_t handler, void* data)
{
	if(!__identifyHead(handler))return NULL;
	return skiList_delete(handler, ((PulleyNode_t*)handler)->prev, data);
}

skiPosition_t skiList_popFront(skiHandler_t handler, void* data)
{
	if(!__identifyHead(handler))return NULL;
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
	if(!__identifyHead(handler) || !func2)goto list_failed;
	PulleyNode_t* pHead = handler, *cursor = NULL;

	for(cursor = pHead->next; cursor != pHead; cursor = cursor->next){
		if(func2(((ListNode_t*)cursor)->data, arg))break;
	}

	return func2(NULL, arg);
list_failed:
	return 0;
}

void skiList_clear(skiHandler_t handler, skiFunc1_t clearFunc)
{
	if(!__identifyHead(handler))return;
	ListHead_t* pListHead = handler;
	PulleyNode_t* pHead = handler;
	ListNode_t* pListNode = NULL;

	//while(pHead->next != pHead){
	for(PulleyNode_t* cursor = pHead; cursor->next != pHead;){
		pListNode = (ListNode_t*)cursor->next;
		if(clearFunc && clearFunc(pListNode->data)){
			cursor = (PulleyNode_t*)pListNode;
		}else{
			__deleteNode(pListNode->pulley.prev, pListNode->pulley.next);
			__initNode(&pListNode->pulley);
			pListNode->nodeID = NULL;
			free(pListNode);
		}
	}
	pListNode = NULL;

	pListHead->listSize = 0;
}

void skiList_destroy(skiHandler_t handler)
{
	if(!__identifyHead(handler))return;
	ListHead_t* pListHead = handler;

	skiList_clear(handler, NULL);
	pListHead->headID = NULL;
	free(handler);
}

void* skiList_at(skiHandler_t handler, skiPosition_t pos)
{
	if(!__identifyHead(handler))goto list_failed;
	if(!__identifyNode(handler, pos))goto list_failed;

	return ((ListNode_t*)pos)->data;
list_failed:
	return NULL;
}

skiPosition_t skiList_begin(skiHandler_t handler)
{
	if(!__identifyHead(handler))goto list_failed;
	PulleyNode_t* pos = ((PulleyNode_t*)handler)->next;

	if(pos == handler)goto list_failed;

	return pos;
list_failed:
	return NULL;
}

skiPosition_t skiList_end(skiHandler_t handler)
{
	if(!__identifyHead(handler))goto list_failed;
	PulleyNode_t* pos = ((PulleyNode_t*)handler)->prev;

	if(pos == handler)goto list_failed;

	return pos;
list_failed:
	return NULL;
}

skiPosition_t skiList_next(skiHandler_t handler, skiPosition_t pos)
{
	if(!__identifyHead(handler))goto list_failed;
	if(!__identifyNode(handler, pos))goto list_failed;

	pos = ((PulleyNode_t*)pos)->next;
	if(pos == handler)goto list_failed;

	return pos;
list_failed:
	return NULL;
}

skiPosition_t skiList_prev(skiHandler_t handler, skiPosition_t pos)
{
	if(!__identifyHead(handler))goto list_failed;
	if(!__identifyNode(handler, pos))goto list_failed;

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

static int __copy_node(void* data, void* handler)
{
	if(data)skiList_pushBack(handler, data);
	return 0;
}

skiHandler_t skiList_copy(skiHandler_t handler)
{
	if(!__identifyHead(handler))goto list_failed;
	ListHead_t* pListHead = handler;

	ListHead_t* pListHeadNew = skiList_create(pListHead->dataSize);
	if(pListHeadNew == NULL)goto list_failed;

	skiList_foreach(pListHead, __copy_node, pListHeadNew);

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

#if 0	//for test sort
void skiList_sort_st1(skiHandler_t handler, skiFunc2_t cmpFunc)
{
	if(!__identifyHead(handler) || cmpFunc == NULL)return;
	__insert_sort(handler, cmpFunc);
}
void skiList_sort_st2(skiHandler_t handler, skiFunc2_t cmpFunc)
{
	if(!__identifyHead(handler) || cmpFunc == NULL)return;
	__tim_sort(handler, cmpFunc);
}
#endif

void skiList_sort(skiHandler_t handler, skiFunc2_t cmpFunc)
{
	if(!__identifyHead(handler) || cmpFunc == NULL)return;
	//__quick_sort(((PulleyNode_t*)handler)->next, ((PulleyNode_t*)handler), cmpFunc);
	__merge_sort(handler, ((ListHead_t*)handler)->listSize, cmpFunc);
}

skiPosition_t skiList_arrange(skiHandler_t handler, void* data, skiFunc2_t cmpFunc)
{
	if(!__identifyHead(handler) || !data)goto list_failed;
	ListHead_t* pListHead = handler;
	PulleyNode_t* pHead = &pListHead->pulley;
	PulleyNode_t* cursor = NULL;

	for(cursor = pHead->next; cursor != pHead; cursor = cursor->next){
		if(cmpFunc(data, ((ListNode_t*)cursor)->data) < 0)break;
	}

	return skiList_insert(handler, cursor, data);
list_failed:
	return NULL;
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

