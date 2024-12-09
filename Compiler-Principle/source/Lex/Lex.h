#pragma once
#include <vector>
#include <map>
#include <set>
#define Epsilon 0

struct edge {
	int begin;
	int end;
	std::set<char>factor;

	edge(int a, int b, char p) :begin(a), end(b) {
		factor.insert(p);
	}
};

class Automaton {
public: 
	std::vector<edge>edges;
	int begin;
	int end;
};


Automaton* merge(std::vector<Automaton*>FAS);
Automaton* closure(Automaton* p);
Automaton* connect(Automaton* p, Automaton* q);