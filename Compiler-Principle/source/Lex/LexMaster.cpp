#include "Lex.h"
using namespace std;

int main()
{
	string path;
	try {
		GRC();
		readRE(path);
		processLex();
		map<int, string>finalStates;
		auto totalA = mergeMultiA(finalStates);

		return 0;
	}
	catch (exception e) {
		cout << e.what();
		return 0;
	}

}