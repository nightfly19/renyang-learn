#include <iostream>
using namespace std;

int main() {
    cout << "sizeof(bool)\t" << sizeof(bool) << "\n";
    cout << "sizeof(true)\t" << sizeof(true) << "\n";
    cout << "sizeof(false)\t" << sizeof(false) << "\n";

    cout << "true\t" << static_cast<int>(true) << "\n";
    cout << "false\t" << static_cast<int>(false) << "\n";
 
    system("pwd"); // 可以透過shell去執行指令
    return 0;
}
