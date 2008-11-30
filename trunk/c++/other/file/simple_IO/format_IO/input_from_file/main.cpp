#include <fstream>
#include <iostream>
using namespace std;

int main(int argc,char *argv[]){
	char ch;
	ofstream fout("temp");
	if (!fout){
		cout << "無法寫入檔案\n";
		return 1;
	}
	fout << "Justin" << "\t" << 90 << endl;
	fout << "momor" << "\t" << 80 << endl;
	fout << "Bush" << "\t" << 75;
	fout.close();

	ifstream fin("temp");
	if(!fin){
		cout << "無法讀入檔案\n";
		return 2;
	}

	char name[80];
	int score;

	cout << "Name \tScore\n";
	while(!fin.eof()){
		fin >> name >> score;			// 一次由檔案讀取一行
		cout << name << "\t" << score << endl;
	}
	fin.close();
	system("rm -v temp");
	return 0;
}
