#include <stdio.h> 
#include <fcntl.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <stdlib.h>
#define FILE_LENGTH 0x400

int main(int argc, char *argv[])
{
   int fd;
   void *map_memory;

   /* Open a file to be mapped. */
   fd = open("/tmp/shared_file", O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);	// O_RDWR:Read/Write.O_CREAT:if the file not exist,we will create the file.S_IRUSR:the user can read. S_IWUSR:the user can write
   lseek(fd, FILE_LENGTH+1, SEEK_SET);	// 跳到檔案開頭,offset為FILE_LENGTH,最後的1應該是預留的位置吧
   write(fd, "", 1);	// 把空白字元寫入檔案指標所指到的位置
   lseek(fd, 0, SEEK_SET);	// 跳回檔案的開頭

   /* Create map memory. */
   map_memory = mmap(0, FILE_LENGTH, PROT_WRITE, MAP_SHARED, fd, 0);	// 可寫,shared,記憶體長度為FILE_LENGTH,指定fd
   close(fd);

   /* Write to mapped memory. */
   if (strlen(argv[1]) < FILE_LENGTH)
   {
      sprintf((char *)map_memory, "%s", argv[1]);	// 把結果寫到map_memory中
   }

   sleep(10);

   exit(0);
}
