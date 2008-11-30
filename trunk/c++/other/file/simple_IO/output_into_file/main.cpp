#include <fstream>
#include <iostream>
using namespace std;

int main(int argc,char *argv[]){
	char ch;
	if (argc != 2){
		cout << "指令: write <filename>" << endl;
		return 1;
	}

	ofstream fout(argv[1]);
	if (!fout){
		cout << "檔案I/O失敗" << endl;
		return 2;
	}

	cout << "輸入文字,以'q'字元結束" << endl;
	do {
		cin.get(ch);	// 由使用者輸入串流了讀取一個字元
		if (ch == 'q'){	// 使用'q'字元當作結束字元
			break;
		}
		fout.put(ch);	// 把這一個字元放入檔案中
	} while (1);
	
	fout.close();
	return 0;
}
