#include <stdio.h>
#include <stdlib.h>

int main(void) {
    int *ptr = malloc(sizeof(int)); 

    printf("空間位置:%X\n", ptr);
    printf("空間儲存值：%d\n", *ptr);

    *ptr = 200; 

    printf("空間位置:%X\n", ptr);
    printf("空間儲存值：%d\n", *ptr);

    free(ptr);
        
    return 0;
}
