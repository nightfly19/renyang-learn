#include <stdio.h>
#include <unistd.h>

int main(int argc,char **argv)
{
	int ch;
	opterr = 0;
	// 表示a的部分還有額外的參數
	while((ch = getopt(argc,argv,"a:bcde"))!= -1)
		switch(ch)
		{
			case 'a':
				// optarg會指到額外的字串位址
				printf("option a:'%s'\n",optarg);
				break;
			case 'b':
				printf("option b :b\n");
				break;
			default:
				printf("other option :%c\n",ch);
		}
	printf("optopt +%c\n",optopt);
}
