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


}

void prepareLex()
{
	try {
		cout << "输入词法路径" << endl;
		cin >> path;
		cout << "输入代码路径" << endl;
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