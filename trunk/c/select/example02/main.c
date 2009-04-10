 #include <sys/time.h>
 #include <stdio.h>
 #include <sys/types.h>
 #include <sys/stat.h>
 #include <fcntl.h>
 #include <assert.h>

int main ()
{
  int keyboard;
  int ret,i;
  char c;
  fd_set readfd;
  struct timeval timeout;
  keyboard = open("/dev/tty",O_RDONLY | O_NONBLOCK);
  assert(keyboard>0);
//	printf("readfd=%c",readfd);
  while(1)
    {
	printf("keyboard=%d\n",keyboard);
	printf("readfd= %d\n",readfd);
  timeout.tv_sec=1;
  timeout.tv_usec=0;
  FD_ZERO(&readfd);
  FD_SET(keyboard,&readfd);
  ret=select(keyboard+1,&readfd,NULL,NULL,&timeout);
	printf("FD_ISSET(keyboard,&readfd)=%d\n",FD_ISSET(keyboard,&readfd));
  if(FD_ISSET(keyboard,&readfd))
    {
	//printf("keyboard=%s",&keyboard);

      i=read(keyboard,&c,1);
          if('\n'==c)
          continue;
      printf("hehethe input is %c\n",c);
    
       if ('q'==c)
       break;

      }
  }
}





