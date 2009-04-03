#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int ac, char **av)
{
	char *str;
	int len;

	// snprintf(NULL, 0, fmtstring, /* 其它參數 */ );
	// 相對於sprintf安全的多
	len = snprintf(NULL, 0, "%s %d", *av, ac);
	printf("this string has length %d\n", len);

	if (!(str = malloc((len + 1) * sizeof(char))))
		return EXIT_FAILURE;

	// 把後面的變數轉換成字串設定到str指標上
	// 而len+1是後面總合長度的上限
	// 超過上限的部分會被刪掉,不會印出來
	len = snprintf(str, len + 1, "%s %d", *av, ac);
	printf("%s %d\n", str, len);

	free(str);

	return EXIT_SUCCESS;
}
