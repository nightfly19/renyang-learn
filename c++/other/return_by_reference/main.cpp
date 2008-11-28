/*
 * Name:
 * Copyright:
 * Author:
 * Date: 27/11/08 20:08
 * Description: learn how to use "return by reference"
 */
#include <iostream> 
#include <string> 
using namespace std; 

string& foo(); // 透過reference傳回值，速度較快

int main() { 
 string &str = foo(); // 對應的必需要是參考變數宣告

 cout << "address: " << &str 
 << endl << str << endl; 

 delete &str; 

 return 0; 
} 

string& foo() { 
 string *s = new string("This is caterpillar speaking."); 

 cout << "address: " << s 
 << endl << *s << endl; 

 return *s; 
}
