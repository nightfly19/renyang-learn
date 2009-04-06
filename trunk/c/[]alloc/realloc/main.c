/* realloc example: rememb-o-matic */
#include <stdio.h>
#include <stdlib.h>

int main ()
{
  int input,n;
  int count=0;
  int * numbers = NULL;

  do {
     printf ("Enter an integer value (0 to end): ");
     scanf ("%d", &input);
     // 個數加1
     count++;
     // count*sizeof(int)表示到目前為止所宣告的大小
     numbers = (int*) realloc (numbers, count * sizeof(int));
     // 當numbers傳回值為NULL時,表示要求記憶體空間失敗
     if (numbers==NULL)
       { puts ("Error (re)allocating memory"); exit (1); }
     // 設定目前count的值
     numbers[count-1]=input;
  } while (input!=0);

  printf ("Numbers entered: ");
  for (n=0;n<count;n++)
     printf ("%d ",numbers[n]);
  printf("\n");
  free (numbers);

  return 0;
}
