#include <stdio.h>
#include <stdlib.h>

struct person // 定義一個名為person的結構
{
char *first_name;
char *second_name;
};

int main()
{
  // 這裡只是宣告一個指標變數
  struct person *yang;
  // 這裡還要宣告結構的空間
  yang = malloc(sizeof(struct person));;
  yang->first_name = "fang";
  yang->second_name = "second_name";
  printf("the first name:%s\nthe second_name:%s\n",yang->first_name,yang->second_name);
  return 0;
}
