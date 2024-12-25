#include "Lex.h"
#include <fstream>
using namespace std;
string path;
string codeFilePath;
int main()
{
	try {
		cout << "输入词法路径" << endl;
		cin >> path;
		cout << "输入代码路径" << endl;
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

		// 读取代码文件 
		readCode(codeFilePath);
		// 模拟扫描文件过程 
		cin.ignore();
		while (1) {
			NametabItem word;
			int res = readNext(totalA, newFinalStates, word);
			/* 对读取到的词word进行处理... */
			if (res == SYN_ERROR) {
				cout << "已退出扫描。" << endl;
				break;
			}
			else if (res == FINISHED) {
				cout << "扫描已完成！" << endl;
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