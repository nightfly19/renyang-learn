#include <fstream>
#include <iostream>
using namespace std;

int main(int argc,char *argv[]){
	if (argc != 3){
		cout << "指令: copy <input file> <output file>" << endl;
		return 1;
	}
	ifstream fin(argv[1]);
	ofstream fout(argv[2]);
	if (!fin){
		cout << "無法讀入目地檔" << endl;
		return 1;
	}
	if (!fout){
		cout << "無法輸出目地檔" << endl;
		fin.close();		// 因為上面開啟成功，所以，現在要關掉
		return 2;
	}
	char ch;
	fin.unsetf(ios::skipws);	// 設定為不忽略空白
	while (!fin.eof()){
		fin >> ch;
		if (ch >= 97 && ch <=122){
			ch -= 32;
		}
		if (!fin.eof()){
			fout << ch;
		}
	}
	fin.close();
	fout.close();
	return 0;
}
