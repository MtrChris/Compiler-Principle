#include "Lex.h"
using namespace std;

extern bitset<MAXCH> recognizeCh;


Automaton* simplifyDFA(Automaton* oldA, map<int, string>oldS, map<int, string>newS)
{
	vector<unordered_set<int>>stateSet;
	stateSet.push_back(unordered_set<int>());
	stateSet.push_back(unordered_set<int>());
	for (auto i : oldA->edges) {
		int a = BEGIN(i.first), b = END(i.first);
		if (oldS.find(a) != oldS.end()) {
			stateSet[0].insert(a);
		}
		else
		{
			stateSet[1].insert(a);
		}
		if (oldS.find(b) != oldS.end()) {
			stateSet[0].insert(b);
		}
		else
		{
			stateSet[1].insert(b);
		}
	}
	int size = 0;
	map<int, unordered_set<int>>sets;
	for (; size != stateSet.size();) {
		size = stateSet.size();
		for (auto i : stateSet) {
			int ssize = 0;
			for (char ch = 0; ch < MAXCH; ch++) {
				if (recognizeCh.test(ch)) {
					for (; ssize < stateSet.size();) {
						ssize = stateSet.size();
						sets.clear();
						for (auto j : i) {
							auto e=getEdges(j, oldA->edges);
							for (auto k = e.begin; k != e.end; k++) {

							}
						}

					}
				}
			}
		}
	}
	return NULL;
}