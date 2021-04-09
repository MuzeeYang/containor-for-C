#include "stdio.h"
#include "skiContainor.h"
#include "sys/time.h"
#include "time.h"
#include "pthread.h"
#include "unistd.h"

#define TEST_SIZE (11)
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
	}
	skiHandler_t cp = skiList_copy(hd);
	printf("data ready.\n");
	skiList_foreach(cp, printNode, NULL);
	putchar(10);


	cmpCallCnt = 0;
	printf("insert sort start =====================\n");
	gettimeofday(&tm, NULL);
	start = tm.tv_sec * 1000000 + tm.tv_usec ;// 1000;
	skiList_sort(hd, cmpFunc);
	gettimeofday(&tm, NULL);
	end = tm.tv_sec * 1000000 + tm.tv_usec ;// 1000;
	printf("result: cmpFunc called: %lu, time takes: %ld\n", cmpCallCnt, end - start);
	skiList_foreach(hd, printNode, NULL);
	putchar(10);

	cmpCallCnt = 0;
	printf("merge sort start =====================\n");
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

	skiList_join(hdlr, cp);
	printf("join list[%p] and list[%p]\n", hdlr, cp);
	skiList_foreach(hdlr, printNode, NULL);
	putchar(10);

	skiList_reCap(hdlr, skiList_size(hdlr));
	printf("recapacity list[%p] to size.\n", hdlr);

	a = 123;
	skiList_pushBack(hdlr, &a);
	printf("push %d into list[%p]\n", a, hdlr);
	skiList_foreach(hdlr, printNode, NULL);
	putchar(10);

	skiPosition_t pos = skiList_search(hdlr, &a, cmpFunc);
	printf("get %d's position[%p]\n", a, pos);

	skiHandler_t cutdown = skiList_cut(hdlr, skiList_posNext(hdlr, pos), 5);
	printf("list[%p] cuted into list[%p]\n", hdlr, cutdown);
	skiList_foreach(hdlr, printNode, NULL);
	putchar(10);
	skiList_foreach(cutdown, printNode, NULL);
	putchar(10);

	skiList_destroy(cutdown);
	printf("destroy list[%p] \n", cutdown);

	cutdown = skiList_cut(hdlr, skiList_posPrev(hdlr, pos), 100);
	printf("list[%p] cuted into list[%p]\n", hdlr, cutdown);
	skiList_foreach(hdlr, printNode, NULL);
	putchar(10);
	skiList_foreach(cutdown, printNode, NULL);
	putchar(10);

	skiList_destroy(cutdown);
	printf("destroy list[%p] \n", cutdown);

	printf("positon's data: %d\n", *(int*)skiList_at(pos));

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

	printf("min = %lu, size = %lu, max = %lu: \n", skiMap_min(hd), skiMap_size(hd), skiMap_max(hd));
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

		printf("min = %lu, size = %lu, max = %lu: \n", skiMap_min(hd), skiMap_size(hd), skiMap_max(hd));
		skiMap_foreach(hd, printMapNode, NULL);
		putchar(10);
	}

	skiMap_destroy(hd);
}

struct _st{
	void* hd;
	int i;
};

void* pushtest(void* hd)
{
	struct _st *ptest_st = hd;

	for(int i = 0; i < 100; i++){
		usleep(1000);
		skiQue_push(ptest_st->hd, &ptest_st->i);
	}
	return NULL;
}

void* poptest(void* hd)
{
	int res[TEST_SIZE] = {0};
	int v = 0;
	skiQue_poll(hd, 0);

	while(skiQue_pop(hd, &v)){
		if(v < 0 || v >= TEST_SIZE){
			printf("error no. %d\n", v);
		}else
			res[v]++;
		usleep(100);
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

	skiQue_destroy(hd);
}

void que_test01()
{
	skiHandler_t hd = skiQue_create(sizeof(int), 1);
	//skiHandler_t hd = skiQue_construct(int, 1);
	int v = 123;

	while(v--)
		skiQue_push(hd, &v);

}

void buf_test()
{
	skiHandler_t hdr = skiBuffer_create("Hello world.\n", 13);
	char buf[256] = {0};

	//printf(skiBuffer_at(hdr, 0));

	skiBuffer_depend(hdr, "Hello Buffer.\n", 14);
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
	hdr = skiBuffer_slice(copy, 0, 10);
	//skiBuffer_cut(copy, 0,buf, 10);
	puts(skiBuffer_at(hdr, 0));
	//puts(buf);
	puts(skiBuffer_at(copy, 0));

	printf("==============\n");
	skiBuffer_join(copy, SKIBUFFER_IDX_END, hdr);
	puts(skiBuffer_at(copy, 0));
	//puts(skiBuffer_at(hdr, 0));


	printf("==============\n");
	skiIndex_t idx = skiBuffer_find(copy, 4, "ll", 2);
	puts(skiBuffer_at(copy, idx));


	printf("==============\n");
	skiBuffer_reSize(copy, 5);
	puts(skiBuffer_at(copy, 0));
	skiBuffer_destroy(copy);
}


int main()
{
	//list_test();
	//sort_test();
	//test_map();
	//que_test();

	//que_test01();

	buf_test();

	return 0;
}
