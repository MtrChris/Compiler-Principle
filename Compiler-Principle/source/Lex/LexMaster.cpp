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
		cout << "DFA Before Simplification: " << endl;
		printNFA(totalA, false, &newFinalStates);
		cout << endl;

		map<int, string> simplifiedNewFinalStates;
		totalA = simplifyDFA(totalA, newFinalStates, simplifiedNewFinalStates);
		cout << "DFA After Simplification: " << endl;
		printNFA(totalA, false, &simplifiedNewFinalStates);
		cout << endl;

		return 0;
	}
	catch (exception e) {
		cout << e.what();
		return 0;
	}

}