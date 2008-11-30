#include <iostream>
#include <fstream>
using namespace std;

int main(int argc,char *argv[]){
	ofstream fout("temp",ios::out | ios::binary);
	if (!fout){
		cout << "無法讀取目的檔案" << endl;
		return 1;
	}

	int arr[5] = {1,5,3,4,5};
	fout.write((char*) arr, sizeof(arr));
	fout.close();

	return 0;
}
