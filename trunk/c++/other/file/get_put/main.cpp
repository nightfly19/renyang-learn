#include <iostream> 
#include <fstream> 
using namespace std; 

int main() { 
    ifstream fin("data.txt"); 
    if(!fin) { 
        cout << "可能是data.txt檔案不存在,無法讀取檔案\n"; 
        return 1; 
    } 

    char name[30]; 
    int request, account, score; 

    cout << "輸入選項:" << endl
         << "1) 顯示所有學生與分數" << endl
         << "2) 顯示及格學生與分數" << endl
         << "3) 顯示不及格學生與分數" << endl
         << "4) 離開" << endl;

    while(true) { 
        cout << "\n選項? "; 
        cin >> request; 

	if (request !=1 && request !=2 && request !=3)
		break;
        switch(request) { 
            case 1: 
                cout << "\n所有學生與分數:" << endl;
                break; 
            case 2: 
                cout << "\n及格學生與分數:" << endl;
                break; 
            case 3: 
                cout << "\n不及格學生與分數:" << endl;
                break; 
        } 

        fin >> account >> name >> score; // 由檔案中讀取資料
        while(!fin.eof()) { 
            if((request == 1) || (request == 2 && score >= 60) || (request == 3 && score < 60)) 
                cout << account << "\t" << name << "\t" << score << endl; // 列印資料到銀幕

            fin >> account >> name >> score; // 由檔案中讀取下一筆資料
        } 

        fin.clear();	// 重置 eof 
        fin.seekg(0);	// get 指標移至檔案首 
    } 

    fin.close(); 

    return 0; 
}
