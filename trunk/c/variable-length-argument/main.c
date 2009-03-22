#include <stdio.h>
#include <stdarg.h>

void foo(int, ...);  

int main(void) {
    double x = 1.1, y = 2.1, z = 3.9; 
    double a = 3.1, b = 8.2, c = 9.3; 

    puts("三個參數："); 
    foo(3, x, y, z); 

    puts("六個參數："); 
    foo(6, x, y, z, a, b, c); 
    
    return 0;
}

void foo(int i, ...) { 
    double tmp; 
    // va_list:宣告一個特殊的型態(type),在va_start,va_arg與va_end三個巨集(macro)當作參數使用
    va_list num_list;

    // va_start:啟始不定長度引數的巨集
    // 我猜此時會設定num_list的數值,確定的數值i放在後面的參數
    va_start(num_list, i);
    
    int j;
    for(j = 0; j < i; j++) {
        printf("%.1f\n", va_arg(num_list, double)); // 在使用va_arg巨集取出引數內容時,必須指定以何種資料型態取出
    }

    // va_end:終止不定長度引數的巨集
    va_end(num_list); 
}
