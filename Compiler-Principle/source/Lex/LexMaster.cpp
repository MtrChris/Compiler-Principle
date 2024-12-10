#include "Lex.h"
#include <iostream>

using namespace std;

//��ʶ��Ķ�����Զ���֮��Ķ�Ӧ��ϵ
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