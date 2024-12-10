#include "Lex.h"
#include <iostream>

using namespace std;

//已识别的短语和自动机之间的对应关系
map<string, Automaton*>CompleteNFA;

int main()
{
	
	string str;
	cin >> str;
	try {
		auto p = getNFA(str);
		printNFA(p);
		return 0;
	}
	catch (exception e) {
		cout << e.what();
		return 0;
	}

}