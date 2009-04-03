/* vsprintf example */
#include <stdio.h>
#include <stdarg.h>

// 不確定參數個數,format也是一個陣列,每一個對應一個變數,這一個例子是[%s,%d]
// 而...代表的就是後面的參數
void PrintFError (char * format, ...)
{
  char buffer[256];
  va_list args;
  va_start (args, format);	// format表示要arg是要以何種資料形態取出
  // int vsprintf (char * str, const char * format, va_list arg );
  // str表示要寫入的空間指標
  vsprintf (buffer,format, args);	// 把所有的字串塞到buffer所指的空間中
  perror (buffer);	// 把設定好的錯誤訊息字串列印出來,同時會依errno,列印出相對的錯誤訊息
  va_end (args);	// 結束不定參數個數函式的使用
}

int main ()
{
   FILE * pFile;
   char szFileName[]="myfile.txt";
   int firstchar = (int) '#';

   pFile = fopen (szFileName,"r");
   if (pFile == NULL)
     PrintFError ("Error opening '%s'\npFile:%d\n",szFileName,pFile);
   else
   {
     // file successfully open,所以,不會列印出任何錯誤訊息
     fclose (pFile);
   }
   return 0;
}
