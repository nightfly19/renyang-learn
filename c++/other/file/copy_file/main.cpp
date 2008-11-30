#include <iostream>
#include <fstream>
using namespace std;

int main(int argc, char* argv[]){
	char ch;
	if (argc != 3){
		cout << "指令: copy <input> <output>" << endl;
		return 1;
	}

	ifstream fin(argv[1],ios::in | ios::binary);
	ofstream fout(argv[2],ios::out | ios::binary);
	if (!fin){
		cout << "來源檔案開啟失敗" << endl;
		return 1;
	}

	if (!fout){
		cout << "目的檔案開啟失敗" << endl;
		fin.close();
		return 2;
	}

	while(!fin.eof()){
		fin.get(ch);
		if (!fin.eof()){
			fout.put(ch);
		}
	}
	fin.close();
	fout.close();
	return 0;
}
