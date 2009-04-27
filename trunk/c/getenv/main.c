/* getenv example: getting path */
#include <stdio.h>
#include <stdlib.h>

int main ()
{
  char * pPath;
  pPath = getenv ("PATH");
  if (pPath!=NULL)
    printf ("The current path is: %s",pPath);
  return 0;
}
