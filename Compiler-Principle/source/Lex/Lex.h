#pragma once
#include <vector>
#include <map>
#include <bitset>
#define Epsilon 1
using uint64 =unsigned long long int;
#define BEGIN(x) ((int)((uint64)x>>32))
#define EDGE(x,y) (((uint64)x<<32)+(uint64)y)
#define END(x) ((int)x)

class Automaton {
public: 
	std::map<uint64,std::bitset<256>>edges;
	int begin;
	int end;
	void edgeMerge(const std::map<uint64, std::bitset<256>>&);
	Automaton(char p);
	Automaton();
};


Automaton* merge(std::vector<Automaton*>FAS);
Automaton* closure(Automaton* p);
Automaton* connect(Automaton* p, Automaton* q);