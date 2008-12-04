#include <iostream>
using namespace std;

class father{
	public:
		father(){	// 建構子
			cout << "I am the father" << endl;
		}

		father(int x){
			cout << "I am the other father" << endl;
		}
};

class son:public father{
	public:
		son(){		// 若沒有指定預設是在執行此建構子前，先執行父類別的沒有參數的建構子
			cout << "I am the son" << endl;
		}

		son(int x) : father(x){	// 在執行此建構子前，會先執行父類別的father(int)的這一個建構子
			cout << "I am the other son" << endl;
		}

		son(int x,int y){	// 若沒有指定預設是在執行此建構子前，先執行父類別的沒有參數的建構子
			cout << "I am the super son" << endl;
		}
};

int main(int argc, char *argv[]){
	cout << "==========================" << endl;
	son *good1 = new son();
	cout << "==========================" << endl;
	son *good2 = new son(1);
	cout << "==========================" << endl;
	son *good3 = new son(1,1);
	cout << "==========================" << endl;
	return 0;
}
