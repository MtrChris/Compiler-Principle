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

		// 读取代码文件 
		readCode(codeFilePath);
		// 模拟扫描文件过程 
		while (1) {
			NametabItem word;
			int res = readNext(totalA, simplifiedNewFinalStates, word);

			/* 对读取到的词word进行处理... */
			
			if (res == SYN_ERROR) {
				cout << "已退出扫描。" << endl;
				break;
			}
			else if (res == FINISHED) {
				cout << "扫描已完成！" << endl;
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