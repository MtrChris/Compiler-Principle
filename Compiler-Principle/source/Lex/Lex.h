#pragma once
#include <vector>
#include <map>
#include <set>

struct state {
	int index;
	bool isFinal;
};

struct edge {
	state* begin;
	state* end;
	std::set<char>factor;
};

class Automaton {
	std::vector<state>states;
	std::vector<edge>edges;
	state* beginer;
	state* end;
};


