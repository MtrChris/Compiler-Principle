#pragma once
#include <vector>
#include <map>
#include <bitset>
#include <string>
#include <iostream>
#include <unordered_set>

#define Epsilon 0

using uint64 =unsigned long long int;
#define BEGIN(x) ((int)((uint64)x>>32))
#define EDGE(x,y) (((uint64)x<<32)+(uint64)y)
#define END(x) ((int)x)
#define OFFSET(x,y) (x+((uint64)y<<32)+y)

class Automaton {
public: 
	std::map<uint64,std::bitset<256>>edges;
	int begin;
	int end;
	void edgeMerge(const std::map<uint64, std::bitset<256>>&);
	Automaton(char p);
	Automaton() :begin(0), end(0) {};
	Automaton(const Automaton* other);

};

struct edges {
	std::map<uint64, std::bitset<256>>::iterator begin;
	std::map<uint64, std::bitset<256>>::iterator end;
};

//LexTool.cpp

Automaton* merge(Automaton* a, Automaton* b);
Automaton* closure(Automaton* p);
Automaton* connect(Automaton* p, Automaton* q);
void printNFA(Automaton* p, bool i = false, std::map<int, std::string>* q = NULL);
edges getEdges(int state, std::map<uint64, std::bitset<256>>& edge);
void standardA(std::map<uint64, std::bitset<256>>& edge);

//REtoNFA

void readRE(std::string path);
void processLex();
Automaton* mergeMultiA(std::map<int, std::string>& finalStates);

//NFAtoDFA.cpp
Automaton* NFAtoDFA(Automaton* oldA, std::map<int, std::string>& finalStates, std::map<int, std::string>& newFinalStates);