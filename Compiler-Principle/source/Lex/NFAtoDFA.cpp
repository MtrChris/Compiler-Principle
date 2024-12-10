#include "Lex.h"

using namespace std;

Automaton* simplifyNFA(Automaton* oldA)
{
	Automaton* newA = new Automaton();
	vector<vector<int>>stateMapping;
	int index = 0;

}