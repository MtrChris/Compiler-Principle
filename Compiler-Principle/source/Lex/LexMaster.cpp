#include "Lex.h"

#include <fstream>

using namespace std;
string path;
string codeFilePath;

map<int, string> finalStates;
Automaton* dfa;
int main()
{
	prepareLex();
	while (1) {
		NametabItem word;
		int res = readNext(word);
		/* �Զ�ȡ���Ĵ�word���д���... */
		if (res == SYN_ERROR) {
			cout << "���˳�ɨ�衣" << endl;
			break;
		}
		else if (res == FINISHED) {
			cout << "ɨ������ɣ�" << endl;
			break;
		}
		cout << "<" << word.name << "," << word.index << ">";
	}


}

void prepareLex()
{
	try {
		cout << "����ʷ�·��" << endl;
		cin >> path;
		cout << "�������·��" << endl;
		cin >> codeFilePath;
		readRE(path);
		processLex();
		map<int, string>f;
		map<int, string>newf;
		auto totalA = mergeMultiA(f);
		dfa = NFAtoDFA(totalA, f, newf);
		dfa = simplifyDFA(dfa, newf, finalStates)
		readCode(codeFilePath);
	}
	catch (exception e) {
		cout << e.what();
	}
}