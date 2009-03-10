// pipe可以讓process之間通訊
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>	// defines pipe()
int fds[2];		// file descriptors of the pipe
			// fds[0]: its read file descriptor
			// fds[1]: its write file descriptor
// 讀取的部分
void do_child()
{
  char c;	// data received from the parent
  int count=0;
  // close the un-needed write-part of the pipe.
  close(fds[1]);
  // reading data from the pipe, and printing it
  while(read(fds[0], &c, 1) > 0)
  {
    putchar(c);
    count++;
  }
  printf("\nChild: #chars = %d\n", count);
  close(fds[0]);
}

// 寫入的部分
void do_parent()
{
  char c;
  // close the un-needed read-part of the pipe.
  close(fds[0]);
  // read user input, and writing it to the pipe.
  while((c=getchar()) > 0)
  {
     // write the character to the pipe.
     write(fds[1], &c, 1);  
  }
  close(fds[1]);
}
int main()
{
  int pid;	// pid of child process, or 0
  int rc;
  // 開啟一個通道,fds[0]是讀取,fds[1]是寫入
  rc = pipe(fds);
  if (rc == -1) {
  	perror("pipe");
	exit(1);
  }
  pid = fork();
  if (pid == 0)
    do_child();
  else
    do_parent();
  return 0;
}
