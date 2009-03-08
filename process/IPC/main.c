#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INPUT 0
#define OUTPUT 1

int main() {
	int file_descriptors[2];
	/*定義子進程號 */
	pid_t pid;
	char buf[256];
	int returned_count;
	/*創建無名管道*/
	pipe(file_descriptors);
	/*創建子進程*/
	if((pid = fork()) == -1) {
		printf("Error in fork\n");
		exit(1);
	}
	/*執行子進程*/
	if(pid == 0) {
		printf("in the spawned (child) process...\n");
		/*子進程向父進程寫數據，關閉管道的讀端*/
		close(file_descriptors[INPUT]);
		write(file_descriptors[OUTPUT], "test data", strlen("test data"));
		exit(0);
	} else {
		/*執行父進程*/
		printf("in the spawning (parent) process...\n");
		/*父進程從管道讀取子進程寫的數據，關閉管道的寫端*/
		close(file_descriptors[OUTPUT]);
		returned_count = read(file_descriptors[INPUT], buf, sizeof(buf));
		printf("%d bytes of data received from spawned process: %s\n",returned_count, buf);
	}
	return 0;
}
