#include <iostream>
#include <fstream>
using namespace std;

int main(int argc, char *argv[]){
	ifstream fin;
	fin.open("main.cpp");

	if (!fin){
		cout << "無法讀取檔案\n";
		return 1;
	}
	else{
		cout << "讀取檔案成功\n";
	}
	fin.close();

	return 0;
}
