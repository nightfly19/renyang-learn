#include <stdlib.h>
#include <stdio.h>
main()
{
	char a[]="1000000000";
	char b[]="1000000000";
	char c[]="ffff";
	printf("a=%d\n",strtol(a,NULL,10));	// 以十進位為底
	printf("b=%d\n",strtol(b,NULL,2));	// 以二進位為底
	printf("c=%d\n",strtol(c,NULL,16));	// 以十六進位為底
	printf("a=%d\n",strtol(a,NULL,0));	// 以十進位為底
}
