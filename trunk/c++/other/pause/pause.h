#ifndef _PAUSE_H
#define _PAUSE_H

#include <stdio.h>

/* implement the getch function */
#include <termios.h>
#include <unistd.h>
#include <assert.h>
#include <string.h>

int getch(void) {
  int c=0;

  struct termios org_opts, new_opts;
  int res=0;
    //----- store old settings -----------
  res=tcgetattr(STDIN_FILENO, &org_opts);
  assert(res==0);
    //---- set new terminal parms --------
  memcpy(&new_opts, &org_opts, sizeof(new_opts));
  new_opts.c_lflag &= ~(ICANON | ECHO | ECHOE | ECHOK | ECHONL | ECHOPRT | ECHOKE | ICRNL);
  tcsetattr(STDIN_FILENO, TCSANOW, &new_opts);
  c=getchar();
    //------ restore old settings ---------
  res=tcsetattr(STDIN_FILENO, TCSANOW, &org_opts);
  assert(res==0);
  return(c);
}
/* end of the implement the getch function */

#define PAUSE printf("Press any key to continue..."); getch();printf("\n");

#endif // _PAUSE_H
