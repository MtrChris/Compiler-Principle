#include "Lex.h"
using namespace std;

extern bitset<MAXCH> recognizeCh;

int findIndex(int s, vector<unordered_set<int>>&stateSet) {
	int i = 0;
	for (; i < stateSet.size(); i++) {
		if (stateSet[i].find(s) != stateSet[i].end()) {
			return i;
		}
	}
	return -1;
}


Automaton* simplify(Automaton* oldA, map<int, string>&oldS, map<int, string>&newS)
{
	vector<unordered_set<int>>stateSet;
	stateSet.push_back(unordered_set<int>());
	map<string,int >SS;
	auto newA = new Automaton();
	for (auto& i : oldA->edges) {
		int a = BEGIN(i.first), b = END(i.first);
		auto c = oldS.find(a);
		if (c != oldS.end()) {
			auto d = SS.find(c->second);
			if (d == SS.end()) {
				int ss = stateSet.size();
				SS[c->second] = ss;
				stateSet.push_back(unordered_set<int>());
				stateSet[ss].insert(a);
			}
			else {
				stateSet[d->second].insert(a);
			}
		}
		else
		{
			stateSet[0].insert(a);
		}
		auto e = oldS.find(b);
		if (e != oldS.end()) {
			auto f = SS.find(e->second);
			if (f == SS.end()) {
				int ss=stateSet.size();
				SS[e->second] = ss;
				stateSet.push_back(unordered_set<int>());
				stateSet[ss].insert(b);
			}
			else {
				stateSet[f->second].insert(b);
			}
		}
		else
		{
			stateSet[0].insert(b);
		}
	}
	int size = 0;
	map<int, unordered_set<int>>sets;
	for (int size = 0; size != stateSet.size();) {
		size = stateSet.size();
		for (int i = 0; i < stateSet.size(); i++) {
			for (int ch = 0; ch < 256; ch++) {
				if (recognizeCh.test(ch)) {
					sets.clear();
					for (auto j : stateSet[i]) {
						auto e = getEdges(j, oldA->edges);
						auto k = e.begin;
						for (; k != e.end; k++) {
							if (k->second.test(ch)) {
								int l = findIndex(END(k->first), stateSet);
								if (l != BEGIN(k->first))
									sets[l].insert(BEGIN(k->first));
								break;
							}
						}
						if (k == e.end) {
							sets[i].insert(j);
						}
					}
					int n = 0;
					for (auto& m : sets) {
						if (n < 1) {
							n++;
							continue;
						}
						for (auto& o : m.second) {
							stateSet[i].erase(o);
						}
						stateSet.push_back(m.second);
					}
				}
			}
		}
	}

	newA->begin = findIndex(0, stateSet);

	for (int i = 0; i< stateSet.size();i++) {
		for(int ch=0;ch<MAXCH;ch++){
			if (recognizeCh.test(ch)) {
				bool a = true,b=a;
				for (auto j : stateSet[i]) {
					if (a) {
						auto k = oldS.find(j);
						if (k != oldS.end()) {
							newS[i] = k->second;
						}
						a = false;
					}
					if (b) {
						auto k = getEdges(j, oldA->edges);
						for (auto f=k.begin; f != k.end; f++) {
							if (f->second.test(ch)) {
								newA->edges[EDGE(i, findIndex(END(f->first), stateSet))].set(ch);
								b = false;
								break;
							}
						}
					}
				}
			}
		}
	}
	return newA;
}