#include "stdio.h"
#include "sys/time.h"
#include "time.h"
#include "pthread.h"
#include "unistd.h"
#include "skiList.h"
#include "skiMap.h"
#include "skiHashMap.h"
#include "skiBuffer.h"
#include "skiQue.h"

#define TEST_SIZE (12)
#define swap_val(_v1, _v2)	do{if((_v1) == (_v2))break; (_v1) ^= (_v2); (_v2) ^= (_v1); (_v1) ^= (_v2);}while(0)
int printNode(void* data, void* out)
{
	if(data)return printf("%d ", *((int*)data));
	return 0;
}

static size_t cmpCallCnt = 0;
int cmpFunc(void* data1, void* data2)
{
	int a = *((int*)data1);
	int b = *((int*)data2);
	cmpCallCnt++;
	return a-b;
}

void sort_test()
{
	struct timeval tm = {0};
	size_t start = 0;
	size_t end = 0;
	int tmp = 0;
	srand(time(NULL));

	printf("=======================  sort test  =====================\n");
	skiHandler_t hd = skiList_construct(int);
	for(int i = 0; i < TEST_SIZE; i++){
		tmp = rand() % (TEST_SIZE * 10);
		skiList_pushBack(hd, &tmp);
		//skiList_arrange(hd, &tmp, cmpFunc);
	}
	skiHandler_t cp = skiList_copy(hd);
	printf("data ready.\n");
	skiList_foreach(cp, printNode, NULL);
	putchar(10);


	cmpCallCnt = 0;
	printf("st1 sort start =====================\n");
	gettimeofday(&tm, NULL);
	start = tm.tv_sec * 1000000 + tm.tv_usec ;// 1000;
	skiList_sort(hd, cmpFunc);
	gettimeofday(&tm, NULL);
	end = tm.tv_sec * 1000000 + tm.tv_usec ;// 1000;
	printf("result: cmpFunc called: %lu, time takes: %ld\n", cmpCallCnt, end - start);
	skiList_foreach(hd, printNode, NULL);
	putchar(10);

	cmpCallCnt = 0;
	printf("st2 sort start =====================\n");
	gettimeofday(&tm, NULL);
	start = tm.tv_sec * 1000000 + tm.tv_usec ;// 1000;
	skiList_sort(cp, cmpFunc);
	gettimeofday(&tm, NULL);
	end = tm.tv_sec * 1000000 + tm.tv_usec ;// 1000;
	printf("result: cmpFunc called: %lu, time takes: %ld\n", cmpCallCnt, end - start);
	skiList_foreach(cp, printNode, NULL);
	putchar(10);

	skiList_destroy(hd);
	skiList_destroy(cp);
}

void list_test()
{
	int a = 100;
	int b = 0;
	skiHandler_t hdlr;
	srand(time(NULL));

	//hdlr = skiList_create(sizeof(int), SKILIST_NOLIMIT);
	hdlr = skiList_construct(int);
	printf("create list[%p]\n", hdlr);

	for(int i = 0; i < 10; i++){
		a = rand() % 100;
		skiList_pushBack(hdlr, &a);
		a = rand() % 100;
		skiList_pushFront(hdlr, &a);
	}

	printf("rand data pushed.\n");
	skiList_foreach(hdlr, printNode, NULL);
	putchar(10);


	skiList_popFront(hdlr, &b);
	printf("pop front data[%d] from front\n", b);
	skiList_foreach(hdlr, printNode, NULL);
	putchar(10);


	a = rand() % 100;
	skiList_pushFront(hdlr, &a);
	printf("pushed data[%d] into front\n", a);
	skiList_foreach(hdlr, printNode, NULL);
	putchar(10);

	skiHandler_t cp = skiList_copy(hdlr);
	printf("list[%p] copied to list[%p]\n", hdlr, cp);
	a = 123;
	skiList_pushBack(cp, &a);
	printf("push %d into list[%p]\n", a, cp);
	skiList_foreach(cp, printNode, NULL);
	putchar(10);

	skiList_sort(cp, cmpFunc);
	printf("list[%p] sorted.\n", cp);
	skiList_foreach(cp, printNode, NULL);
	putchar(10);
	
	skiList_reverse(cp);
	printf("list[%p] reversed.\n", cp);
	skiList_foreach(cp, printNode, NULL);
	putchar(10);

	a = 123;
	skiList_pushBack(hdlr, &a);
	printf("push %d into list[%p]\n", a, hdlr);
	skiList_foreach(hdlr, printNode, NULL);
	putchar(10);

	skiPosition_t pos = skiList_search(hdlr, &a, cmpFunc);
	printf("get %d's position[%p]\n", a, pos);

	printf("positon's data: %d\n", *(int*)skiList_at(hdlr, pos));

	skiList_destroy(hdlr);
	printf("destroy list[%p] \n", hdlr);
}

static int printMapNode(size_t idx, void* data, void* out)
{
	printf("[%lu: %d], ", idx, *(int*)data);
	return 0;
}

void test_map()
{
	srand(time(NULL));
	int value = 123;
	int key[TEST_SIZE] = {78, 66, 65, 14, 35, 57, 80, 93, 19, 65, 62};
	skiHandler_t hd = skiMap_construct(int);

	printf("{");
	for(int i = 0; i < TEST_SIZE; i++){
		key[i] = rand() % (TEST_SIZE*10);
		printf("%d, ", key[i]);
		value += key[i];
		skiMap_push(hd, key[i], &value);
		//putchar(10);
		//printRBTree4dbg(hd);
		//putchar(10);
	}
	printf("}");
	putchar(10);

	//printf("min = %lu, size = %lu, max = %lu: \n", skiMap_min(hd), skiMap_size(hd), skiMap_max(hd));
	skiMap_foreach(hd, printMapNode, NULL);
	putchar(10);

	for(int i = 0; i < TEST_SIZE; i++){
		int j = rand() % TEST_SIZE;
		swap_val(key[i], key[j]);
	}
	int key_tmp[TEST_SIZE] = {62, 93, 57, 14, 80, 66, 65, 78, 19, 65, 35};

	printf("{");
	for(int i = 0; i < TEST_SIZE; i++){
		//key[i] = key_tmp[i];
		printf("%d, ", key[i]);
	}
	printf("}");
	putchar(10);

	int* valtmp = skiMap_at(hd, 39);
	if(valtmp){
		printf("39: %d\n", *valtmp);
		*valtmp = 1024;
	}


	for(int i = 0; i < TEST_SIZE; i++){
		skiMap_pop(hd, key[i], &value);
		printf("deleted [%d: %d] ==> ", key[i], value);

		//printf("min = %lu, size = %lu, max = %lu: \n", skiMap_min(hd), skiMap_size(hd), skiMap_max(hd));
		skiMap_foreach(hd, printMapNode, NULL);
		putchar(10);
	}

	skiMap_destroy(hd);
}

struct _st{
	void* hd;
	int i;
};

int pauseFunc(void* arg)
{
	//if(arg)
		//printf("%s push waiting\n", __func__);
	//else
		//printf("%s pop waiting\n", __func__);
	return 0;
}

void* pushtest(void* hd)
{
	struct _st *ptest_st = hd;

	for(int i = 0; i < 1000; i++){
		skiQue_push(ptest_st->hd, &ptest_st->i, pauseFunc, &i);
	}
	return NULL;
}

void* poptest(void* hd)
{
	int res[TEST_SIZE] = {0};
	int v = 0;
	size_t total = 0;
	//skiQue_poll(hd, 0, NULL, NULL);
	//sleep(1);

	while(total < TEST_SIZE*1000){
		//printf("total: %d\n", total);
		skiQue_pop(hd, &v, pauseFunc, NULL);
		if(v < 0 || v >= TEST_SIZE){
			printf("error no. %d\n", v);
		}else
			res[v]++;
		total++;
	}

	for(int i = 0; i < TEST_SIZE; i++){
		printf("%d's cnt %d,\t", i, res[i]);
	}
	putchar(10);
	return NULL;
}

void que_test()
{
	skiHandler_t hd = skiQue_construct(int, 1000);
	pthread_t pid[TEST_SIZE];

	pthread_t popid;
	pthread_create(&popid, NULL, poptest, hd);

	struct _st test_st[TEST_SIZE];
	for(int i = 0; i < TEST_SIZE; i++){
		test_st[i].hd = hd;
		test_st[i].i = i;
		pthread_create(&pid[i], NULL, pushtest, &test_st[i]);
	}

	for(int i = 0; i < TEST_SIZE; i++){
		pthread_join(pid[i], NULL);
	}


	pthread_join(popid, NULL);

	//skiQue_destroy(hd);
}

void que_test01()
{
	skiHandler_t hd = skiQue_create(sizeof(int), 1);
	//skiHandler_t hd = skiQue_construct(int, 1);
	int v = 123;

	while(v--)
		skiQue_push(hd, &v, pauseFunc, NULL);

}

static int printSubBuffer(void* buffer, void* arg)
{
	skiHandler_t* buf = buffer;
	if(buf && *buf)
		printf("%s\n", skiBuffer_at(*buf, 0));
	return 0;
}

void buf_test()
{
	skiHandler_t hdr = skiBuffer_create("Hello world.\n", 13);
	char buf[256] = {0};

	//printf(skiBuffer_at(hdr, 0));

	skiBuffer_append(hdr, "Hello Buffer.\n", 14);
	//printf(skiBuffer_at(hdr, 0));
	skiBuffer_truncate(hdr, NULL, 8);
	printf("==============\n");

	skiBuffer_insert(hdr, SKIBUFFER_IDX_END, "abc!\n", 6);
	puts(skiBuffer_at(hdr, 0));

	skiHandler_t copy = skiBuffer_copy(hdr);
	skiBuffer_destroy(hdr);

	printf("==============\n");
	skiBuffer_cut(copy, 7, buf, 2);
	puts(buf);
	puts(skiBuffer_at(copy, 0));


	printf("==============\n");
	skiIndex_t idx = skiBuffer_find(copy, 4, "ll", 2);
	puts(skiBuffer_at(copy, idx));


	printf("==============\n");
	skiBuffer_resize(copy, 5);
	puts(skiBuffer_at(copy, 0));
	skiBuffer_destroy(copy);

	strcpy(buf, "abcd123abcdefg123hijkm123nopqrstu123vwxyz");
	hdr = skiBuffer_create(buf, strlen(buf));

	printf("%s\n", skiBuffer_at(hdr, 0));
	skiHandler_t list = skiBuffer_split(hdr, "123", 3);
	printf("%x\n", list);

	skiList_foreach(list, printSubBuffer, NULL);

	copy = skiBuffer_create("</a>", 4);
	skiBuffer_destroy(hdr);
	hdr = skiBuffer_join(copy, list);

	printf("%s\n", skiBuffer_at(hdr, 0));
}


static int everyNode(char* key, void* data, void* arg)
{
	if(data)
		printf("key = %s: value = %d\n", key, *(int*)data);
	return 0;
}

static int clearNode(void* key, void* value)
{
	return (*(int*)value) > 3;
}

void hashMap_test()
{
	int i;
	skiHandler_t hd = skiHashMap_create(sizeof(int), 0);

	char* keys[] = {
		"fjdksal",
		"t82o rgja",
		"abcdefg",
		"123456",
		"/ 9,0;ikm. rte4r5td3w srdg",
		"t271	 GV/P;ZCX,.",
		"2805r fjdwsklfsaxvf",
		"8`1905 8itojgksdal'L;",
	};

	for(i = 0; i < sizeof(keys)/sizeof(char*); i++){
		skiHashMap_push(hd, keys[i], &i);
	}

	printf("hashmap size = %d\n", skiHashMap_size(hd));
	skiHashMap_foreach(hd, everyNode, NULL);
	*(int*)skiHashMap_at(hd, "123456") = 123;


	skiHashMap_pop(hd, "123456", &i);
	printf("popped value: %d\n", i);

	printf("============\n");
	skiHashMap_foreach(hd, everyNode, NULL);

	skiHashMap_clear(hd, clearNode);
	printf("============\n");
	skiHashMap_foreach(hd, everyNode, NULL);

	skiHashMap_destroy(hd);
}

int main()
{
	//list_test();
	//sort_test();
	//test_map();
	que_test();
	//que_test01();
	buf_test();
	//hashMap_test();

	return 0;
}
