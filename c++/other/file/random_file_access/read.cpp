#include <iostream> 
#include <fstream> 
#include "Student.h" 
using namespace std; 

int main(int argc, char* argv[]) { 
    Student student; 
    int count = 0, number; 

    if(argc != 2) { 
        cout << "指令: read <filename>" << endl; 
        return 1; 
    } 

    ifstream fin(argv[1], ios::in | ios::binary); 
    if(!fin) { 
        cerr << "無法讀取檔案" << endl; 
        return 1; 
    } 

    while(true) {	// 這一個迴圈主要是測試一共有幾筆資料
        fin.read(reinterpret_cast<char *> (&student),sizeof(Student)); 
        if(!fin.eof()) 
            count++; 
        else 
            break; 
    } 
    fin.clear(); 

    cout << "輸入學號(1-" << count << ")"  << endl
         << "輸入0離開"; 

    while(true) { 
        cout << "\n學號? "; 
        cin >> number; 
        if(number == 0) 
            break; 
        else if(number > count) { 
            cout << "輸入學號(1-" << count << ")" << endl; 
            continue; 
        } 
                                                                                
        cout << "\n學號\t姓名\t\t分數" << endl; 
                                                                                
        fin.seekg((number - 1) * sizeof(Student));	// 跳到指定的位址
        fin.read(reinterpret_cast<char*> (&student),sizeof(Student));	// 在指定的位址讀出Student的資料結構,並存到student的位址中
        cout << student.studyNumber << "\t" 
             << student.name << "\t\t" 
             << student.score << endl; 
    } 

    fin.close(); 

    return 0; 
}
