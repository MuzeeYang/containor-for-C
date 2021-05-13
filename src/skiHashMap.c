#include "skiContainor.h"

static char skiHashMapID;

typedef struct _HashMapNode{
	skiHashCode_t hashCode;
	size_t valueOffset;
	struct _HashMapNode* next;
	char key[0];
}HashMapNode_t;

typedef struct _HashMapHead{
	size_t mapSize;
	size_t dataSize;
	int capIdx;
	void* headID;
	HashMapNode_t** array;
}HashMapHead_t;

#define memset __builtin_memset
#define memcpy __builtin_memcpy
#define memmove __builtin_memmove
#define memcmp __builtin_memcmp

#define __identifyHead(_pHead) ((_pHead) && ((HashMapHead_t*)_pHead)->headID == &skiHashMapID)
static const skiHashCode_t hashSeed[] = {
1313131313, 131313131, 13131313, 1313131, 131313, 13131, 1313, 131, 31
};

static const skiHashCode_t hashCap[] = {
23, 61, 83, 113, 151, 211, 281, 379, 509, 683, 911, //under 1k
1217, 1627, 2179, 2909, 3881, 6907, 9209, //under 10k
12281, 16381, 21841, 29123, 38833, 51787, 69061, 92083, //under 10w
122777, 163729, 218357, 291143, 388211, 517619, 690163, 999983, //under 100w
1226959, 1635947, 2181271
};

static const int CAP_TABLE_LENGTH = sizeof(hashCap)/sizeof(skiHashCode_t);

#define SKI_INT_HAS_B0(_v) (~(((((_v) & 0x7F7F7F7F) + 0x7F7F7F7F) | (_v)) | 0x7F7F7F7F))

static int inline _byte_cnt_1(char c)
{
	int cnt = 0;
	while(c){c &= c-1; cnt++;}
	return cnt;
}

static int inline _get_new_cap_idx(size_t sz)
{
	int i = 0;
	for(i = 0; i < CAP_TABLE_LENGTH; i++)
		if(hashCap[i] >= sz)break;

	return i;
}

size_t skiStrlen(char* str)
{
	skiU32_t* cur = (skiU32_t*)str;
	size_t len = 0;
	while(!SKI_INT_HAS_B0(*cur))cur++;
	len = (char*)cur - str;

	str = (char*)cur;
	while(*str++)len++;
	return len;
}

int skiStrcmp(char* str1, char* str2)
{
	return memcmp(str1, str2, skiStrlen(str1) + 1);
}

static size_t _bkdrHash(char *str, skiHashCode_t* out)
{
	skiHashCode_t hash = 0;
	size_t len = 0;
	char c;
	while((c = *str++)){
		hash = hash*hashSeed[_byte_cnt_1(c)] + c;
		len++;
	}
	if(out)*out = hash;
	return len + 1;
}

skiHandler_t skiHashMap_create(size_t dataSize, size_t capacity)
{
	HashMapHead_t* pMapHead = malloc(sizeof(HashMapHead_t));
	if(pMapHead == NULL)goto hash_failed;
	memset(pMapHead, 0, sizeof(HashMapHead_t));

	pMapHead->capIdx = _get_new_cap_idx(capacity);
	pMapHead->array = malloc(sizeof(HashMapNode_t*)*hashCap[pMapHead->capIdx]);
	if(pMapHead->array == NULL){
		free(pMapHead);
		goto hash_failed;
	}
	memset(pMapHead->array, 0, sizeof(HashMapNode_t*)*hashCap[pMapHead->capIdx]);

	pMapHead->dataSize = dataSize;
	pMapHead->headID = &skiHashMapID;

	return pMapHead;
hash_failed:
	return NULL;
}

static void _cap_increase(HashMapHead_t* pMapHead)
{
	if(pMapHead->capIdx == CAP_TABLE_LENGTH)return;
	HashMapNode_t** array = malloc(sizeof(HashMapNode_t*)*hashCap[pMapHead->capIdx+1]);
	if(array == NULL)return;

	memset(array, 0, sizeof(HashMapNode_t*)*hashCap[pMapHead->capIdx+1]);
	HashMapNode_t* cursor = NULL;
	int arrIdx = 0;
	for(int i = 0; i < hashCap[pMapHead->capIdx]; i++){
		while((cursor = pMapHead->array[i])){
			pMapHead->array[i] = cursor->next;
			arrIdx = cursor->hashCode % hashCap[pMapHead->capIdx+1];
			cursor->next = array[arrIdx];
			array[arrIdx] = cursor;
		}
	}

	free(pMapHead->array);
	pMapHead->array = array;
	pMapHead->capIdx++;
}

size_t skiHashMap_push(skiHandler_t handler, char* key, void* value)
{
	if(!__identifyHead(handler))goto hash_failed;
	HashMapHead_t* pMapHead = handler;

	if(pMapHead->mapSize > ((hashCap[pMapHead->capIdx]*3) >> 2))
		_cap_increase(pMapHead);
	
	skiHashCode_t hashCode = 0;
	size_t keyLength = _bkdrHash(key, &hashCode);
	int arrIdx = hashCode % hashCap[pMapHead->capIdx];

	HashMapNode_t* cursor = NULL;
	for(cursor = pMapHead->array[arrIdx]; cursor != NULL; cursor = cursor->next){
		if(cursor->hashCode == hashCode
		&& cursor->valueOffset == keyLength
		&& 0 == memcmp(cursor->key, key, keyLength))
			goto hash_failed;
	}

	HashMapNode_t* pMapNode = malloc(sizeof(HashMapNode_t)+pMapHead->dataSize+keyLength);
	if(pMapNode == NULL)goto hash_failed;
	memset(pMapNode, 0, sizeof(HashMapNode_t)+pMapHead->dataSize+keyLength);

	memcpy(pMapNode->key, key, keyLength);
	memcpy(pMapNode->key + keyLength, value, pMapHead->dataSize);
	pMapNode->hashCode = hashCode;
	pMapNode->valueOffset = keyLength;

	pMapNode->next = pMapHead->array[arrIdx];
	pMapHead->array[arrIdx] = pMapNode;

	return ++(pMapHead->mapSize);
hash_failed:
	return 0;
}

size_t skiHashMap_pop(skiHandler_t handler, char* key, void* value)
{
	if(!__identifyHead(handler))goto hash_failed;
	HashMapHead_t* pMapHead = handler;

	skiHashCode_t hashCode = 0;
	size_t keyLength = _bkdrHash(key, &hashCode);
	int arrIdx = hashCode % hashCap[pMapHead->capIdx];

	HashMapNode_t* cursor = pMapHead->array[arrIdx];
	HashMapNode_t* target = NULL;
	if(cursor == NULL)goto hash_failed;

	if(cursor->next == NULL){
		target = cursor;
		if(target->hashCode == hashCode
		&& target->valueOffset == keyLength
		&& 0 == memcmp(target->key, key, keyLength)){
			pMapHead->array[arrIdx] = NULL;
			target->next = NULL;
			goto hash_out;
		}
	}

	for(; cursor->next != NULL; cursor = cursor->next, target = NULL){
		target = cursor->next;
		if(target->hashCode == hashCode
		&& target->valueOffset == keyLength
		&& 0 == memcmp(target->key, key, keyLength)){
			cursor->next = target->next;
			target->next = NULL;
			break;
		}
	}
	if(target == NULL)goto hash_failed;

hash_out:
	if(value)memcpy(value, target->key + target->valueOffset, pMapHead->dataSize);
	free(target);
	return pMapHead->mapSize--;
hash_failed:
	return 0;
}

void* skiHashMap_at(skiHandler_t handler, char* key)
{
	if(!__identifyHead(handler))goto hash_failed;
	HashMapHead_t* pMapHead = handler;

	skiHashCode_t hashCode = 0;
	size_t keyLength = _bkdrHash(key, &hashCode);
	int arrIdx = hashCode % hashCap[pMapHead->capIdx];

	HashMapNode_t* cursor = pMapHead->array[arrIdx];
	if(cursor && cursor->next == NULL)goto hash_out;

	for(; cursor != NULL; cursor = cursor->next){
		if(cursor->hashCode == hashCode
		&& cursor->valueOffset == keyLength
		&& 0 == memcmp(cursor->key, key, keyLength))
			goto hash_out;
	}

hash_out:
	return cursor->key + cursor->valueOffset;
hash_failed:
	return NULL;
}

int skiHashMap_foreach(skiHandler_t handler, skiFunc3_t func3, void* arg)
{
	if(!__identifyHead(handler))goto hash_failed;
	HashMapHead_t* pMapHead = handler;

	HashMapNode_t* cursor = NULL;
	for(int i = 0; i < hashCap[pMapHead->capIdx]; i++){
		for(cursor = pMapHead->array[i]; cursor != NULL; cursor = cursor->next)
			if(func3(cursor->key, cursor->key+cursor->valueOffset, arg))
				goto hash_out;
	}

hash_out:
	return func3("", NULL, arg);
hash_failed:
	return 0;
}

size_t skiHashMap_size(skiHandler_t handler)
{
	if(!__identifyHead(handler))goto hash_failed;
	HashMapHead_t* pMapHead = handler;

	return pMapHead->mapSize;
hash_failed:
	return 0;
}

void skiHashMap_clear(skiHandler_t handler, skiFunc2_t clearFunc)
{
	if(!__identifyHead(handler))goto hash_failed;
	HashMapHead_t* pMapHead = handler;

	HashMapNode_t* cursor = NULL;
	HashMapNode_t* target = NULL;
	for(int i = 0; i < hashCap[pMapHead->capIdx]; i++){
		if(pMapHead->array[i] == NULL)continue;
		cursor = pMapHead->array[i];
		while(!clearFunc || !clearFunc(cursor->key, cursor->key+cursor->valueOffset)){
			pMapHead->array[i] = cursor->next;
			//cursor->next = NULL;
			free(cursor);
			if(pMapHead->array[i])cursor = pMapHead->array[i];
			else break;
		}
		if(pMapHead->array[i] == NULL)continue;

		for(cursor = pMapHead->array[i]; cursor->next != NULL;){
			target = cursor->next;
			if(clearFunc && clearFunc(target->key, target->key+target->valueOffset)){
				cursor = target;
			}else{
				cursor->next = target->next;
				free(target);
			}
		}
	}

hash_failed:
	return;
}

void skiHashMap_destroy(skiHandler_t handler)
{
	if(!__identifyHead(handler))goto hash_failed;
	HashMapHead_t* pMapHead = handler;

	skiHashMap_clear(pMapHead, NULL);

	free(pMapHead->array);
	pMapHead->headID = NULL;
	free(pMapHead);
hash_failed:
	return;
}


