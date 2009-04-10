#include <stdio.h>
#include <stdlib.h>

int main(void) {
    int size = 0; 

    printf("請輸入陣列長度："); 
    scanf("%d", &size); 
    int *arr = malloc(size * sizeof(int));

    int i;
    
    printf("顯示元素值：\n");
    for(i = 0; i < size; i++) {
        printf("arr[%d] = %d\n", i, *(arr+i));
    } 
        
    printf("指定元素值：\n"); 
    for(i = 0; i < size; i++) { 
        printf("arr[%d] = ", i); 
        scanf("%d" , arr + i); 
    } 

    printf("顯示元素值：\n");
    for(i = 0; i < size; i++) {
        printf("arr[%d] = %d\n", i, *(arr+i));
    } 

    free(arr); 
    
    return 0;
}
