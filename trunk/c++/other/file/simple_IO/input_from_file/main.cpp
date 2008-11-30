#include <fstream>
#include <iostream>
using namespace std;

int main(int argc,char *argv[]){
	char ch;
	if (argc != 2){
		cout << "指令: read <檔案名稱>" << endl;
		return 1;
	}

	ifstream fin;
	fin.open(argv[1]);

	if (!fin){
		cout << "檔案I/O失敗" << endl;
		return 2;
	}

	while (!fin.eof()){	// 判斷是不是檔案最後
		fin.get(ch);	// 由檔案中讀取出一個字元
		cout << ch;	// 把這一個字元放到輸出串流
	}

	fin.close();

	return 0;
}
