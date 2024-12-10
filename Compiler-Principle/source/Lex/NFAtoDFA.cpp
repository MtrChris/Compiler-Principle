#include "Lex.h"

using namespace std;

extern bitset<256> recognizeCh;


void findEqualS(map<uint64,bitset<256>>&edges,unordered_set<int>&states,int p)
{
	auto i = getEdges(p, edges);

	for (auto j = i.begin; j != i.end; j++) {
		if (j->second.test(Epsilon) && states.find(END(j->first)) == states.end()) {
			states.insert(END(j->first));
			findEqualS(edges , states, END(j->first));
		}
	}
}

void findTargetS(map<uint64, bitset<256>>& edges, unordered_set<int>& states, int p, char c)
{
	auto i = getEdges(p, edges);
	for (auto j = i.begin; j != i.end; j++) {
		if (j->second.test(c) && states.find(END(j->first)) == states.end()) {
			states.insert(END(j->first));
		}
	}
}

bool aIsSubsetOfB(unordered_set<int>&a, unordered_set<int>&b) 
{
	for (auto i : a) {
		if (b.find(i) == b.end())
			return false;
	}
	return true;
}

int isSubsetOfOne(vector<unordered_set<int>>& ss, unordered_set<int>& s) 
{
	for (int i = 0; i < ss.size();i++) {
		if (aIsSubsetOfB(s, ss[i])) {
			return i;
		}
	}
	return -1;
}

void checkFinal(const unordered_set<int>&states,map<int, string>& finalStates, map<int, string>& newFinalStates,int i)
{   
	map<int, string>::iterator k;
	for (auto j : states) {
		if ((k = finalStates.find(j)) != finalStates.end()) {
			newFinalStates.insert(pair<int, string>{i, k->second});
		}
	}
}

Automaton* NFAtoDFA(Automaton* oldA,map<int,string>& finalStates, map<int, string>& newFinalStates)
{
	Automaton* newA = new Automaton();
	vector<unordered_set<int>>stateMapping;
	unordered_set<int>states;

    standardA(oldA->edges);

	states.insert(oldA->begin);
	findEqualS(oldA->edges, states, oldA->begin);
	stateMapping.push_back(states);
	states.clear();

	for (int index = 0; index < stateMapping.size();index++) {
		for (int ch = 0; ch < 256; ch++) {
			if (recognizeCh.test(ch)) {
				for (auto j : stateMapping[index]) {
					findTargetS(oldA->edges, states, j, ch);
				}
				for (auto j : states) {
					unordered_set<int>k;
					findEqualS(oldA->edges, k, j);
					states.insert(k.begin(),k.end());
				}
				if (states.size() > 0) {
					int k = isSubsetOfOne(stateMapping, states);
					if (k == -1) {
						int p = stateMapping.size();
						newA->edges[EDGE(index, p)].set(ch);
						checkFinal(states, finalStates, newFinalStates, p);
						stateMapping.push_back(states);
					}
					else {
						newA->edges[EDGE(index, k)].set(ch);
					}
					states.clear();
				}
			}
		}
	}
	delete oldA;
	return newA;
}