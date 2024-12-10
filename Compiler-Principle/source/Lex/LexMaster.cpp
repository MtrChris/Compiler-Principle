#include "Lex.h"
using namespace std;

int main()
{
	string path= "D:\\myfile\\code\\Compiler-Principle\\Compiler-Principle\\source\\Lex\\Lex.txt";
	try {
		readRE(path);
		processLex();
		map<int, string>finalStates;
		auto totalA = mergeMultiA(finalStates);
		//printNFA(totalA);

		map<int, string>newFinalStates;
		totalA = NFAtoDFA(totalA,finalStates,newFinalStates);

		printNFA(totalA, false, &newFinalStates);
		return 0;
	}
	catch (exception e) {
		cout << e.what();
		return 0;
	}

}