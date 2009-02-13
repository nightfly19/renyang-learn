#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>


int main (int argc, char **argv)
{
  int c;

  while (1)
    {
      struct option long_options[] =
        {
          {"add", 1, 0, 0},
          {"append", 0, 0, 0},
          {"delete", 1, 0, 0},
/* 返回字符c,等同 -c 選項 */
          {"create", 0, 0, 'c'},
          {"file", 1, 0, 0},
/* 數組結束 */
          {0, 0, 0, 0}
        };
      /* getopt_long stores the option index here. */
      int option_index = 0;

      c = getopt_long (argc, argv, "abc:d:",
                       long_options, &option_index);

      /* Detect the end of the options. */
      if (c == -1)
        break;

      switch (c)
        {
        case 0:
          printf ("option %s", long_options[option_index].name);
          if (optarg)
            printf (" with arg %s\n", optarg);
          break;

        case 'a':
          puts ("option -a\n");
          break;

        case 'b':
          puts ("option -b\n");
          break;

/* 可能是-c --creat參數指出來的 */
        case 'c':
          printf ("option -c with value `%s'\n", optarg);
          break;

        case 'd':
          printf ("option -d with value `%s'\n", optarg);
          break;
        }
    }

  exit (0);
}

