#include <stdio.h>

struct // 這一個結構沒有名稱，即匿名結構
{
  int id;
  char *path;
  int frames;
}
kas_animations [] = // kas_animations就是上面建立的結構，而下面就是初始化
{
  {01,"first",12},
  {02,"second",24},
  {03,"third",36}
}; // 記得要在最後加分號

struct person // 定義一個名為person的結構
{
char *first_name;
char *second_name;
};

int main()
{
  int i;
  for (i=0;i<3;i++){
    printf("id:%d\tpath:%s\tframes:%d\n",kas_animations[i].id,kas_animations[i].path,kas_animations[i].frames);
  }

  struct person ren = {"Fang","renyang"}; // 初始化一個person型別的ren變數，在c中必需要加struct，但是，在c++中可以省略
  printf("The first name is %s, and the second name is %s\n",ren.first_name,ren.second_name);
  return 0;
}
