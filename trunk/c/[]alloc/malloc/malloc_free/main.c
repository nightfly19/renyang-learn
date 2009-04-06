#include <stdio.h>
#include <stdlib.h>

int main()
{
	// 若不給指標預設值的話,ptr不會預設是NULL
	double *ptr=NULL;
	if (ptr==NULL)
		printf("1. the pointer is null\n");
	// malloc是以byte為基本單位
	// 配置完後,ptr本來指到NULL,會變成沒有指到NULL
	ptr = malloc (100*sizeof(int));
	if (ptr==NULL)
		printf("2. the pointer is null\n");
	// 印出指標變數的大小,固定就是4bytes
	printf("the size of ptr is %d\n",sizeof(ptr));
	// 我們只能知道第一個變數的大小,沒有辦法知道一整個我們所分配到的空間的大小,所以通常用一個變數記錄我們分配的大小
	printf("the size of *ptr is %d\n",sizeof(*ptr));

	// 清除指到的記憶體空間,清除之後,ptr不會自動指到NULL
	free(ptr);
	if (ptr==NULL)
		printf("3. the pointer is null\n");
	
	ptr = NULL;
	if (ptr==NULL)
		printf("4. the pointer is null\n");
	
	return 0;
}
