#pragma once
#include <vector>
#include <map>
#include <bitset>
#include <string>
#include <iostream>

#define Epsilon 0

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

struct edges {
	map<uint64, bitset<256>>::iterator begin;
	map<uint64, bitset<256>>::iterator end;
};

//LexTool.cpp

Automaton* merge(Automaton* a, Automaton* b);
Automaton* closure(Automaton* p);
Automaton* connect(Automaton* p, Automaton* q);
void printNFA(Automaton* p);

//REtoNFA

void readRE(std::string path);
void processLex();
void GRC();
Automaton* mergeMultiA(std::map<int, std::string>& finalStates);

//NFAtoDFA.cpp
Automaton* simplifyNFA(Automaton*);