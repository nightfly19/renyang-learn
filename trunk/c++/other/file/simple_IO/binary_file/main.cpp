#include <fstream>
#include <iostream>
#include <iomanip>
using namespace std;

int main(int argc,char *argv[]){
	char ch;
	int count = 0;
	if (argc != 2){
		cout << "指令: read <filename>" << endl;
		return 1;
	}
	ifstream fin(argv[1],ios::in | ios::binary);
	if (!fin){
		cout << "無法讀取檔案" << endl;
		return 2;
	}

	while (!fin.eof()){
		fin.get(ch);
		if (ch < 0){
			ch = ~ch;	// 負數取補數
		}
		cout << setw(2) << hex << (int)ch << " ";
		count++;
		if (count > 16){	// 換行
			cout << endl;
			count = 0;
		}
	}
	cout << endl;
	fin.close();
	return 0;
}
