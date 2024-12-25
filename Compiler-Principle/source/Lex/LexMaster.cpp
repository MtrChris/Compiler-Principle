#include "Lex.h"
#include <fstream>
using namespace std;

int main()
{
	// string path= "D:\\myfile\\code\\Compiler-Principle\\Compiler-Principle\\source\\Lex\\Lex.txt";
	string path = "E:\\VS Code Files\\Project\\CompilerPrinciple_Project\\source\\Lex\\Lex.txt";
	string codeFilePath = "E:\\VS Code Files\\Project\\CompilerPrinciple_Project\\source\\Lex\\code.txt";
	try {
		readRE(path);
		processLex();
		map<int, string>finalStates;
		auto totalA = mergeMultiA(finalStates);
		//printNFA(totalA);

		map<int, string>newFinalStates;
		totalA = NFAtoDFA(totalA,finalStates,newFinalStates);
		cout << "DFA Before Simplification: " << endl;
		printNFA(totalA, false, &newFinalStates);
		cout << endl;

		map<int, string> simplifiedNewFinalStates;
		totalA = simplifyDFA(totalA, newFinalStates, simplifiedNewFinalStates);
		cout << "DFA After Simplification: " << endl;
		printNFA(totalA, false, &simplifiedNewFinalStates);
		cout << endl;

		// ��ȡ�����ļ� 
		readCode(codeFilePath);
		// ģ��ɨ���ļ����� 
		while (1) {
			NametabItem word;
			int res = readNext(totalA, simplifiedNewFinalStates, word);

			/* �Զ�ȡ���Ĵ�word���д���... */
			
			if (res == SYN_ERROR) {
				cout << "���˳�ɨ�衣" << endl;
				break;
			}
			else if (res == FINISHED) {
				cout << "ɨ������ɣ�" << endl;
				break;
			}
		}

		return 0;
	}
	catch (exception e) {
		cout << e.what();
		return 0;
	}

}