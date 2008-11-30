#include <iostream> 
#include <fstream> 
#include "Student.h" 
using namespace std; 

int main(int argc, char* argv[]) { 
    if(argc != 2) { 
        cout << "指令: create <filename>" << endl; 
        return 1; 
    } 

    ofstream fout(argv[1], ios::binary); 

    if(!fout) { 
        cerr << "目的檔開啟失敗" << endl; 
        return 1; 
    } 

    int count; 
    cout << "要建立幾筆資料？ "; 
    cin >> count; 

    Student student = {0, "", 0.0}; 

    for(int i = 0; i < count; i++) { 
        fout.write(reinterpret_cast<const char*> (&student), 
            sizeof(Student)); 
    } 

    fout.close(); 

    return 0; 
}
