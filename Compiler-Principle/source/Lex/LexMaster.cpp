#include "Lex.h"
#include <fstream>
using namespace std;
string path;
string codeFilePath;
int main()
{
	try {
		cout << "����ʷ�·��" << endl;
		cin >> path;
		cout << "�������·��" << endl;
		cin >> codeFilePath;
		readRE(path);
		processLex();
		map<int, string>finalStates;
		auto totalA = mergeMultiA(finalStates);
		//printNFA(totalA);

		map<int, string>newFinalStates;
		totalA = NFAtoDFA(totalA,finalStates,newFinalStates);
		//cout << "DFA Before Simplification: " << endl;
		//cout << endl;

		//map<int, string> simplifiedNewFinalStates;
		//totalA = simplifyDFA(totalA, newFinalStates, simplifiedNewFinalStates);
		//cout << "DFA After Simplification: " << endl;
		//printNFA(totalA, false, &simplifiedNewFinalStates);
		//cout << endl;

		// ��ȡ�����ļ� 
		readCode(codeFilePath);
		// ģ��ɨ���ļ����� 
		cin.ignore();
		while (1) {
			NametabItem word;
			int res = readNext(totalA, newFinalStates, word);
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

		return 0;
	}
	catch (exception e) {
		cout << e.what();
		return 0;
	}

}