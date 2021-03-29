#include "stdio.h"
#include "skiContainor.h"
#include "sys/time.h"
#include "time.h"

#define TEST_SIZE (100000)
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
	//skiList_foreach(cp, printNode, NULL);
	putchar(10);


	cmpCallCnt = 0;
	printf("insert sort start =====================\n");
	gettimeofday(&tm, NULL);
	start = tm.tv_sec * 1000000 + tm.tv_usec ;// 1000;
	skiList_sort(hd, cmpFunc);
	gettimeofday(&tm, NULL);
	end = tm.tv_sec * 1000000 + tm.tv_usec ;// 1000;
	printf("result: cmpFunc called: %lu, time takes: %ld\n", cmpCallCnt, end - start);
	//skiList_foreach(hd, printNode, NULL);
	putchar(10);

	cmpCallCnt = 0;
	printf("merge sort start =====================\n");
	gettimeofday(&tm, NULL);
	start = tm.tv_sec * 1000000 + tm.tv_usec ;// 1000;
	skiList_sort(cp, cmpFunc);
	gettimeofday(&tm, NULL);
	end = tm.tv_sec * 1000000 + tm.tv_usec ;// 1000;
	printf("result: cmpFunc called: %lu, time takes: %ld\n", cmpCallCnt, end - start);
	//skiList_foreach(cp, printNode, NULL);
	putchar(10);

	skiList_destroy(hd, NULL);
	skiList_destroy(cp, NULL);
}

int main()
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

	skiList_destroy(cutdown, NULL);
	printf("destroy list[%p] \n", cutdown);

	cutdown = skiList_cut(hdlr, skiList_posPrev(hdlr, pos), 100);
	printf("list[%p] cuted into list[%p]\n", hdlr, cutdown);
	skiList_foreach(hdlr, printNode, NULL);
	putchar(10);
	skiList_foreach(cutdown, printNode, NULL);
	putchar(10);

	skiList_destroy(cutdown, NULL);
	printf("destroy list[%p] \n", cutdown);

	printf("positon's data: %d\n", *(int*)skiList_at(pos));

	skiList_destroy(hdlr, NULL);
	printf("destroy list[%p] \n", hdlr);

	//sort_test();

	return 0;
}
