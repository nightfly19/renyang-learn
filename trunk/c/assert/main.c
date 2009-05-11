/* assert example */
#include <stdio.h>
#include <assert.h>

int main ()
{
  FILE *datafile;
  datafile = fopen ("file.dat","r");
  assert (datafile);

  fclose (datafile);

  return 0;
}
