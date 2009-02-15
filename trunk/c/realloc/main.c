#include <stdio.h>
#include <stdlib.h>

int main(void) {
    int size = 0; 

    printf("請輸入陣列長度："); 
    scanf("%d", &size); 
    int *arr1 = malloc(size * sizeof(int));

    int i;
            
    printf("指定元素值：\n"); 
    for(i = 0; i < size; i++) { 
        printf("arr1[%d] = ", i); 
        scanf("%d" , arr1 + i); 
    } 

    printf("顯示元素值：\n");
    for(i = 0; i < size; i++) {
        printf("arr1[%d] = %d\n", i, *(arr1+i));
    } 

    int *arr2 = realloc(arr1, sizeof(int) * size * 2);
    printf("顯示元素值：\n");
    for(i = 0; i < size * 2; i++) {
        printf("arr2[%d] = %d\n", i, *(arr2+i));
    }
	
    printf("arr1 address: %d\n", arr1);
    printf("arr2 address: %d\n", arr1);
	
    // free(arr1);	// 在realloc時,已把arr1執行free的動作了
    free(arr2);

    return 0;
}
