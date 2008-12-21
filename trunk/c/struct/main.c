#include <stdio.h>

struct
{
  int id;
  char *path;
  int frames;
}
kas_animations [] =
{
  {01,"first",12},
  {02,"second",24},
  {03,"third",36}
};

int main()
{
  int i;
  for (i=0;i<3;i++){
    printf("id:%d\tpath:%s\tframes:%d\n",kas_animations[i].id,kas_animations[i].path,kas_animations[i].frames);
  }
  return 0;
}
