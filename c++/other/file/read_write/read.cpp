#include <iostream>
#include <fstream>
using namespace std;

int main(int argc,char *argv[]){
	int arr[5];

	ifstream fin("temp",ios::in | ios::binary);
	if (!fin){
		cout << "無法讀取來源檔案" << endl;
		return 2;
	}

	fin.read((char*) arr,sizeof(arr));
	cout << "arr: ";
	for (int i=0;i<5;i++){
		cout << arr[i] << ' ';
	}
	cout << endl;
	fin.close();
	return 0;
}
