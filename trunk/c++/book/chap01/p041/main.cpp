
/*
 * show the "Type a word:______
 * then using six backchar to put the cursor on the first _
 */
#include <iostream>
using namespace std;

int main()
{
	string input;

	cout << "Type a word:_____" << "\b\b\b\b\b";

	cin >> input;

	cout << "You typed " << input << endl;

	return 0;
}
