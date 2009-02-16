#include <stdio.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

int
main(void) {
    fd_set rfds;
    struct timeval tv;
    int retval;

   /* Watch stdin (fd 0) to see when it has input. */
    FD_ZERO(&rfds);	// 清空rfds與所有文件指標的關聯
    FD_SET(STDIN_FILENO, &rfds);

   /* Wait up to five seconds. */
    tv.tv_sec = 5;
    tv.tv_usec = 0;

   retval = select(1, &rfds, NULL, NULL, &tv);	// 1表示最大的文件描述值加1,等待rfds有輸入值時,或timeout時,回傳數值
    /* Don't rely on the value of tv now! */

   if (retval == -1)	// 出現錯誤,或發生中斷
        perror("select()");
    else if (retval)
        printf("Data is available now.\n");
        /* FD_ISSET(0, &rfds) will be true. */
    else
        printf("No data within five seconds.\n");

   return 0;
}
