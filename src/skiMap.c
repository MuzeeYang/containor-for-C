#include "skiContainor.h"

static char skiMapID;

typedef struct _StalkNode{
	struct _StalkNode* parent;
	struct _StalkNode* left;
	struct _StalkNode* right;
	struct {
		unsigned char color:1;
		unsigned char branch:2;
	}sts;
}StalkNode_t;

typedef struct _MapNode{
	StalkNode_t stalk;
	skiIndex_t key;
	char value[0];
}MapNode_t;

typedef struct _MapHead{
	StalkNode_t* root;
	size_t dataSize;
	size_t mapSize;
	void* headID;
	skiIndex_t minKey;
	skiIndex_t maxKey;
}MapHead_t;

#define MAPNODE_RED		(0)
#define MAPNODE_BLACK	(1)

#define MAPNODE_ROOT	(0)
#define MAPNODE_LEFT	(1)
#define MAPNODE_RIGHT	(2)

#define memset __builtin_memset
#define memcpy __builtin_memcpy

#define __identifyHead(_pHead) ((_pHead) && ((MapHead_t*)_pHead)->headID == &skiMapID)
#define __swap_val(_v1, _v2) do{typeof(_v1) _vt = (_v1); (_v1) = (_v2); (_v2) = _vt;}while(0)
#define __getSingleChild(_node) ((_node)->left? (_node)->left: (_node)->right)
#define __getMinKey(_k1, _k2)	((_k1)<(_k2)? (_k1): (_k2))
#define __getMaxKey(_k1, _k2)	((_k1)>(_k2)? (_k1): (_k2))

static void inline __initNode(StalkNode_t* node)
{
	memset(node, 0, sizeof(StalkNode_t));
}

static void inline __setRoot(StalkNode_t* node)
{
	if(node){
		node->parent = NULL;
		node->sts.branch = MAPNODE_ROOT;
		node->sts.color = MAPNODE_BLACK;
	}
}

static void inline __addNode_left(StalkNode_t* parent, StalkNode_t* node)
{
	if(parent)parent->left = node;
	if(node){
		node->sts.branch = MAPNODE_LEFT;
		node->parent = parent;
	}
}

static void inline __addNode_right(StalkNode_t* parent, StalkNode_t* node)
{
	if(parent)parent->right = node;
	if(node){
		node->sts.branch = MAPNODE_RIGHT;
		node->parent = parent;
	}
}

static void inline __change_child(StalkNode_t* parent, StalkNode_t* node, StalkNode_t* child)
{
	if(node->sts.branch == MAPNODE_ROOT)__setRoot(child);
	else if(node->sts.branch == MAPNODE_RIGHT)
		__addNode_right(parent, child);
	else if(node->sts.branch == MAPNODE_LEFT)
		__addNode_left(parent, child);
}

static void inline __removeNode(StalkNode_t* node, StalkNode_t* child)
{
	__change_child(node->parent, node, child);
	__initNode(node);
}

static void inline __rotateNode_left(StalkNode_t* node)
{
	if(node->right == NULL)return;
	StalkNode_t* right = node->right;
	__change_child(node->parent, node, right);
	__addNode_right(node, right->left);
	__addNode_left(right, node);
}

static void inline __rotateNode_right(StalkNode_t* node)
{
	if(node->left == NULL)return;
	StalkNode_t* left = node->left;
	__change_child(node->parent, node, left);
	__addNode_left(node, left->right);
	__addNode_right(left, node);
}

static void inline __replaceNode(StalkNode_t* orgNode, StalkNode_t* newNode)
{
	//memcpy(newNode, orgNode, sizeof(StalkNode_t));
	__addNode_left(newNode, orgNode->left);
	__addNode_right(newNode, orgNode->right);
	__change_child(orgNode->parent, orgNode, newNode);
	newNode->sts = orgNode->sts;
	__initNode(orgNode);
}


skiHandler_t skiMap_create(size_t dataSize)
{
	MapHead_t* pMapHead = malloc(sizeof(MapHead_t));
	if(pMapHead == NULL)goto map_failed;
	
	memset(pMapHead, 0, sizeof(MapHead_t));
	pMapHead->dataSize = dataSize;
	pMapHead->headID = &skiMapID;
	pMapHead->maxKey = 0;
	pMapHead->minKey = -1;

	return pMapHead;
map_failed:
	return NULL;
}

static MapNode_t* _insert_deal(MapHead_t* pHead, StalkNode_t* node)
{
	if(pHead->root == NULL){
		node->left = node->right = NULL;
		__setRoot(node);
		pHead->root = node;
		return NULL;
	}

	StalkNode_t* refer = pHead->root;
	while(1){
		if(((MapNode_t*)node)->key < ((MapNode_t*)refer)->key){
			if(refer->left){
				refer = refer->left;
				continue;
			}else{
				__addNode_left(refer, node);
				break;
			}
		}else if(((MapNode_t*)node)->key > ((MapNode_t*)refer)->key){
			if(refer->right){
				refer = refer->right;
				continue;
			}else{
				__addNode_right(refer, node);
				break;
			}
		}else{
			__replaceNode(refer, node);
			return (MapNode_t*)refer;
		}
	}

	node->sts.color = MAPNODE_RED;
	//_insert_adjust(pHead, node);
	return NULL;
}

static void _insert_adjust(MapHead_t* pHead, StalkNode_t* node)
{
	StalkNode_t* parent, *gparent, *uncle;
	while(node->sts.branch != MAPNODE_ROOT){
		parent = node->parent;
		gparent = parent->parent;
		//parent is black
		if(parent->sts.color == MAPNODE_BLACK)break;

		//parent is red
		if(parent->sts.branch == MAPNODE_LEFT){
			uncle = gparent->right;
			//case 1: uncle is red
			if(uncle && uncle->sts.color == MAPNODE_RED){
				parent->sts.color = MAPNODE_BLACK;
				uncle->sts.color = MAPNODE_BLACK;
				gparent->sts.color = MAPNODE_RED;
				node = gparent;
				continue;
			}

			//case 2: node is right child, uncle is black
			if(node->sts.branch == MAPNODE_RIGHT){
				__rotateNode_left(parent);
				node = parent;
				parent = node->parent;
			}

			//case 3: node is left child, uncle is black
			if(node->sts.branch == MAPNODE_LEFT){
				gparent->sts.color = MAPNODE_RED;
				parent->sts.color = MAPNODE_BLACK;
				__rotateNode_right(gparent);
				node = parent;
				break;
			}
		}else if(parent->sts.branch == MAPNODE_RIGHT){
			uncle = gparent->left;
			//case 1: uncle is red
			if(uncle && uncle->sts.color == MAPNODE_RED){
				parent->sts.color = MAPNODE_BLACK;
				uncle->sts.color = MAPNODE_BLACK;
				gparent->sts.color = MAPNODE_RED;
				node = gparent;
				continue;
			}

			//case 2: node is left child, uncle is black
			if(node->sts.branch == MAPNODE_LEFT){
				__rotateNode_right(parent);
				node = parent;
				parent = node->parent;
			}

			//case 3: node is right child, uncle is black
			if(node->sts.branch == MAPNODE_RIGHT){
				gparent->sts.color = MAPNODE_RED;
				parent->sts.color = MAPNODE_BLACK;
				__rotateNode_left(gparent);
				node = parent;
				break;
			}
		}
	}

	if(node->sts.branch == MAPNODE_ROOT){
		__setRoot(node);
		pHead->root = node;
	}
}

size_t skiMap_push(skiHandler_t handler, skiIndex_t key, void* value)
{
	if(!__identifyHead(handler) || !value)goto map_failed;
	MapHead_t* pMapHead = handler;
	MapNode_t* insResult = NULL;

	MapNode_t* pMapNode = malloc(sizeof(MapNode_t) + pMapHead->dataSize);
	if(pMapNode == NULL)goto map_failed;

	__initNode((StalkNode_t*)pMapNode);
	pMapNode->key = key;
	memcpy(pMapNode->value, value, pMapHead->dataSize);
	insResult = _insert_deal(pMapHead, (void*)pMapNode);
	if(insResult){
		if(pMapNode->stalk.sts.branch == MAPNODE_ROOT)pMapHead->root = (StalkNode_t*)pMapNode;
		memcpy(value, insResult->value, pMapHead->dataSize);
		free(insResult);
		insResult = NULL;
	}else{
		_insert_adjust(pMapHead, (void*)pMapNode);
		pMapHead->minKey = __getMinKey(key, pMapHead->minKey);
		pMapHead->maxKey = __getMaxKey(key, pMapHead->maxKey);
		pMapHead->mapSize++;
	}

	return pMapHead->mapSize;

map_failed:
	return 0;
}

static StalkNode_t* _search_find(MapHead_t* pHead, skiIndex_t key)
{
	StalkNode_t* refer = pHead->root;
	if(refer == NULL)goto map_failed;

	while(1){
		if(((MapNode_t*)refer)->key > key){
			if(refer->left){
				refer = refer->left;
				continue;
			}else goto map_failed;
		}else if(((MapNode_t*)refer)->key < key){
			if(refer->right){
				refer = refer->right;
				continue;
			}else goto map_failed;
		}else break;
	}

	return refer;

map_failed:
	return NULL;
}

static void _delete_aim(MapHead_t* pHead, StalkNode_t* node)
{
	if(node->left && node->right){
		StalkNode_t* cursor = node->right, temp;
		while(cursor->left)cursor = cursor->left;

		// swap node
		temp = *cursor;
		*cursor = *node;
		*node = temp;
		if(node->parent == node)__change_child(cursor, node, node);
		else __change_child(node->parent, node, node);
		__change_child(cursor->parent, cursor, cursor);
		__addNode_left(node, node->left);
		__addNode_right(node, node->right);
		__addNode_left(cursor, cursor->left);
		__addNode_right(cursor, cursor->right);

		//root confirm
		if(cursor->sts.branch == MAPNODE_ROOT)pHead->root = cursor;
	}
}

static void _delete_adjust(MapHead_t* pHead, StalkNode_t* node)
{
	//case 0: node is red
	if(node->sts.color == MAPNODE_RED)return;

	//case 1: node has single red child
	StalkNode_t* tip = __getSingleChild(node);
	if(tip && tip->sts.color == MAPNODE_RED){
		tip->sts.color = MAPNODE_BLACK;
		return;
	}

	StalkNode_t* parent, *sibling, *lnephew, *rnephew;
	tip = node;
	while(node->sts.branch != MAPNODE_ROOT){

		if(node->sts.branch == MAPNODE_LEFT){
			//case 2: sibling is red
			parent = node->parent;
			sibling = parent->right;
			if(sibling->sts.color == MAPNODE_RED){
				parent->sts.color = MAPNODE_RED;
				sibling->sts.color = MAPNODE_BLACK;
				__rotateNode_left(parent);
				tip = sibling;
				sibling = parent->right;
			}

			//case 3: sibling is black, nephews are black;
			lnephew = sibling->left;
			rnephew = sibling->right;
			if((lnephew == NULL || lnephew->sts.color == MAPNODE_BLACK) &&
				(rnephew == NULL || rnephew->sts.color == MAPNODE_BLACK)){
				sibling->sts.color = MAPNODE_RED;
				if(parent->sts.color == MAPNODE_BLACK){
					node = parent;
					continue;
				}else{
					parent->sts.color = MAPNODE_BLACK;
					break;
				}
			}

			//case 4: sibling is black, right nephew is red
			if(rnephew && rnephew->sts.color == MAPNODE_RED){
				sibling->sts.color = parent->sts.color;
				parent->sts.color = MAPNODE_BLACK;
				rnephew->sts.color = MAPNODE_BLACK;
				__rotateNode_left(parent);
				tip = sibling;
				break;
			}

			//case 5: sibling is black, left nephew is red;
			if(lnephew && lnephew->sts.color == MAPNODE_RED){
				sibling->sts.color = MAPNODE_RED;
				lnephew->sts.color = MAPNODE_BLACK;
				__rotateNode_right(sibling);
				tip = lnephew;
				continue;
			}

		}else{

			//case 6: sibling is red
			parent = node->parent;
			sibling = parent->left;
			if(sibling->sts.color == MAPNODE_RED){
				parent->sts.color = MAPNODE_RED;
				sibling->sts.color = MAPNODE_BLACK;
				__rotateNode_right(parent);
				tip = sibling;
				sibling = parent->left;
			}

			//case 7: sibling is black, nephews are black
			lnephew = sibling->left;
			rnephew = sibling->right;
			if((lnephew == NULL || lnephew->sts.color == MAPNODE_BLACK) &&
				(rnephew == NULL || rnephew->sts.color == MAPNODE_BLACK)){
				sibling->sts.color = MAPNODE_RED;
				if(parent->sts.color == MAPNODE_BLACK){
					node = parent;
					continue;
				}else{
					parent->sts.color = MAPNODE_BLACK;
					break;
				}
			}

			//case 8: sibling is black, left nephew is red
			if(lnephew && lnephew->sts.color == MAPNODE_RED){
				sibling->sts.color = parent->sts.color;
				parent->sts.color = MAPNODE_BLACK;
				lnephew->sts.color = MAPNODE_BLACK;
				__rotateNode_right(parent);
				tip = sibling;
				break;
			}

			//case 9: sibling is black, right nephew is red
			if(rnephew && rnephew->sts.color == MAPNODE_RED){
				sibling->sts.color = MAPNODE_RED;
				rnephew->sts.color = MAPNODE_BLACK;
				__rotateNode_left(sibling);
				tip = rnephew;
				continue;
			}
		
		}

	}

	//if(tip && tip->sts.branch == MAPNODE_ROOT)pHead->root = tip;
	while(tip->parent)tip = tip->parent;
	pHead->root = node;

}

static void _delete_deal(MapHead_t* pHead, StalkNode_t* node)
{
	//StalkNode_t* child = (StalkNode_t*)((size_t)node->right | (size_t)node->left);
	StalkNode_t* child = __getSingleChild(node);
	if(node->sts.branch == MAPNODE_ROOT)pHead->root = child;
	__removeNode(node, child);
}

static skiIndex_t _prev_index(StalkNode_t* node)
{
	if(node->left){
		node = node->left;
		while(node->right)node = node->right;
	}else if(node->parent){
		if(node->sts.branch == MAPNODE_RIGHT)
			node = node->parent;
		else return 0;
	}
	else return 0;

	return ((MapNode_t*)node)->key;
}

static skiIndex_t _next_index(StalkNode_t* node)
{
	if(node->right){
		node = node->right;
		while(node->left)node = node->left;
	}else if(node->parent){
		if(node->sts.branch == MAPNODE_LEFT)
			node = node->parent;
		else return 0;
	}
	else return 0;

	return ((MapNode_t*)node)->key;
}

size_t skiMap_pop(skiHandler_t handler, skiIndex_t key, void* value)
{
	if(!__identifyHead(handler))goto map_failed;
	MapHead_t* pMapHead = handler;

	MapNode_t* node = (MapNode_t*)_search_find(pMapHead, key);
	if(node == NULL)goto map_failed;

	if(pMapHead->maxKey == node->key) pMapHead->maxKey = _prev_index((StalkNode_t*)node);
	if(pMapHead->minKey == node->key) pMapHead->minKey = _next_index((StalkNode_t*)node);

	_delete_aim(pMapHead, (StalkNode_t*)node);
	_delete_adjust(pMapHead, (StalkNode_t*)node);
	_delete_deal(pMapHead, (StalkNode_t*)node);

	if(value)memcpy(value, node->value, pMapHead->dataSize);
	free(node);

	return pMapHead->mapSize--;
map_failed:
	return 0;
}

void* skiMap_at(skiHandler_t handler, skiIndex_t key)
{
	if(!__identifyHead(handler))goto map_failed;

	MapNode_t* node = (MapNode_t*)_search_find(handler, key);
	if(node == NULL)goto map_failed;

	return node->value;

map_failed:
	return NULL;
}

static void _foreach_deal(StalkNode_t* root, skiFunc3_t func3, void* arg)
{
	if(root->left)_foreach_deal(root->left, func3, arg);
	func3(((MapNode_t*)root)->key, ((MapNode_t*)root)->value, arg);
	if(root->right)_foreach_deal(root->right, func3, arg);
}

void skiMap_foreach(skiHandler_t handler, skiFunc3_t func3, void* arg)
{
	if(!__identifyHead(handler) || !func3 || !((MapHead_t*)handler)->root)
		goto map_failed;

	_foreach_deal(((MapHead_t*)handler)->root, func3, arg);

map_failed:
	return;
}

static void _clear_deal(StalkNode_t* root)
{
	if(root->left)_clear_deal(root->left);
	if(root->right)_clear_deal(root->right);
	free(root);
}

void skiMap_clear(skiHandler_t handler)
{
	if(!__identifyHead(handler))goto map_failed;
	MapHead_t* pMapHead = handler;
	if(pMapHead->root) _clear_deal(pMapHead->root);
	pMapHead->root = NULL;
	pMapHead->mapSize = 0;

map_failed:
	return;
}

void skiMap_destroy(skiHandler_t handler)
{
	if(!__identifyHead(handler))goto map_failed;
	skiMap_clear(handler);
	((MapHead_t*)handler)->headID = NULL;
	free(handler);
map_failed:
	return;
}

size_t skiMap_size(skiHandler_t handler)
{
	if(!__identifyHead(handler))goto map_failed;
	return ((MapHead_t*)handler)->mapSize;
map_failed:
	return 0;
}

skiIndex_t skiMap_min(skiHandler_t handler)
{
	if(!__identifyHead(handler))goto map_failed;
	return ((MapHead_t*)handler)->minKey;
map_failed:
	return 0;
}

skiIndex_t skiMap_max(skiHandler_t handler)
{
	if(!__identifyHead(handler))goto map_failed;
	return ((MapHead_t*)handler)->maxKey;
map_failed:
	return 0;
}

#if 0	//debug
void printRBTree4dbg(skiHandler_t handler)
{
	if(!__identifyHead(handler))goto map_failed;
	MapHead_t* pMapHead = handler;
	void* nodeBuf[64] = {0};
	StalkNode_t* node = NULL;

	nodeBuf[0] = pMapHead->root;
	for(int i = 0; i < sizeof(nodeBuf)/sizeof(void*); i++){
		node = nodeBuf[i];
		if(node){
			nodeBuf[2*i+1] = node->left;
			nodeBuf[2*i+2] = node->right;
		}
	}

	int l = 0, j = 0;
	for(int i = 0; i < sizeof(nodeBuf)/sizeof(void*); i++, j++){
		if(j < (1 << l)){
			printf(" ");
		}else{
			printf("\n");
			j = 0;
			l++;
		}

		node = nodeBuf[i];
		if(node){
			printf("%d%c", ((MapNode_t*)node)->key, node->sts.color?'b':'r');
		}else{
			printf("null");
		}

	}

map_failed:
	return 0;
}
#endif
