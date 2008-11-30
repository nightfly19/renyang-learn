#include <fstream>
#include <iostream>
#include <string>
using namespace std;

int main(int argc,char *argv[]){
	string str;

	if (argc != 2){
		cout << "指令: write <filename>" << endl;
		return 1;
	}
	ofstream fout(argv[1]);
	if (!fout){
		cout << "無法寫入檔案" << endl;
		return 1;
	}

	do {
		cout << "$ ";
		// cin >> str;
		getline(cin,str);
		if (str == "q") {
			break;
		}
		fout << str << endl;
	}while(str != "q");

	fout.close();
	return 0;
}
