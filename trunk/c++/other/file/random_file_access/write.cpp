#include <iostream> 
#include <fstream> 
#include "Student.h" 
using namespace std; 

int main(int argc, char* argv[]) { 
    Student student; 
    int count = 0; 

    if(argc < 2) { 
        cerr << "指令: write <filename>"; 
        return 1; 
    } 

    fstream fio(argv[1], ios::in | ios::out | ios::binary); 
    if(!fio) { 
        cerr << "無法讀取檔案" << endl; 
        return 1; 
    } 

    while(true) {	// 這一個迴圈主要是測試一共有多少筆資料
        fio.read(reinterpret_cast<char *> (&student),sizeof(Student));
        if(!fio.eof()) 
            count++; 
        else 
            break; 
    } 

    fio.clear();	// 重置 eof

    cout << "輸入學號(1-" << count << ")"  << endl
         << "輸入0離開"; 

    while(true) { 
        cout << "\n學號? "; 
        cin >> student.studyNumber; 
        if(student.studyNumber == 0) 
            break; 
        
        cout << "輸入姓名, 分數" << endl
             << "? "; 
        cin >> student.name >> student.score;	// 先把要存入的資料先存到student中

        fio.seekp((student.studyNumber - 1) * sizeof(Student));	// 跳到指定的位置,準備存入資料
        fio.write(reinterpret_cast<const char*> (&student),sizeof(Student));	// 寫入區塊資料到檔案中
    } 

    fio.close(); 

    return 0; 
}
