#include <stdio.h>
#include <string.h>

#define MAXDATA 480*640*3

// 建立一個struct用來記錄height,width,與圖像資料(設定最大的空間)
struct matrix
{
	int height, width;
	char data[MAXDATA];
};

int main(int argc,char **argv)
{
	struct matrix test_data;
	char str[] = "Hello World!!";

	bzero(&test_data,sizeof(test_data));

	test_data.height = 480;
	test_data.width = 640;

	memcpy(test_data.data,str,sizeof(str));
	
	printf("%d\n",sizeof(test_data));

	printf("the height:%d\nthe width:%d\nthe string:%s\n",test_data.height,test_data.width,test_data.data);

	return 0;
}
