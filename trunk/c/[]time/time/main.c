/* time example */
#include <stdio.h>
#include <time.h>

int main ()
{
  time_t seconds;

  seconds = time (NULL);
  printf ("%ld seconds since January 1, 1970\n", seconds);
  
  return 0;
}
