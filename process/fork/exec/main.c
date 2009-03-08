#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

char command[256];

int main()
{
	int rtn;	// 子程序的返回數值
	while(1) {
		// 由終端機讀取要執行的命令
		printf(">");
		fgets(command,256,stdin);
		command[strlen(command)-1]=0;
		if (fork()==0) {
			execlp(command,NULL);
			// 如果exec函數返回,表明沒有正常執行命令,列印錯誤訊息
			perror(command);
			exit(errno);
		}
		else
		{
			// 父程序,等待子程序結序,並列印子程序的返回值
			// pid_t wait(int *status);是表示父行程等待其中一個(任何一個)子行程結束
			// 回傳值是表示哪一個子行程，而status是表示當這一個子行程結束時的狀態
			wait(&rtn);
			printf("child process return %d\n",rtn);
		}
	}
	return 0;
}
