#pragma once
#include <vector>
#include <map>
#include <bitset>
#include <string>
#include <iostream>
#include <unordered_set>
#include <algorithm>

#define Epsilon 0
#define MAXCH 256
#define SYN_ERROR -1
#define NOT_FINISHED 0
#define FINISHED 1

using uint64 = unsigned long long int;
//ȡ���
#define BEGIN(x) ((int)((uint64)(x)>>32)) 
//�����յ�ϳɱ�
#define EDGE(x,y) (((uint64)(x)<<32)+(uint64)(y))
//ȡ�յ�
#define END(x) ((int)(x))
//����x�������յ�ͬʱƫ��y
#define OFFSET(x,y) ((uint64)(x)+((uint64)(y)<<32)+y)

class Automaton {
public: 
	std::map<uint64,std::bitset<MAXCH>>edges;
	int begin;
	int end;
	void edgeMerge(const std::map<uint64, std::bitset<MAXCH>>&);
	Automaton(char p);
	Automaton() :begin(0), end(0) {};
	Automaton(const Automaton* other);

};

struct edges {
	std::map<uint64, std::bitset<MAXCH>>::iterator begin;
	std::map<uint64, std::bitset<MAXCH>>::iterator end;
};

struct NametabItem {
	int index;				// ��ţ���0��ʼ 
	std::string name;		// ���֣���abc 
	std::string type;		// ���ͣ���123>abc��123 
	int value;				// ֵ��Ĭ��������0��������Ҫ�޸�
	NametabItem() {};
	NametabItem(int index, std::string name, std::string type, int value) {
		this->index = index;
		this->name = name;
		this->type = type;
		this->value = value;
	}
	void print() {
		std::cout << "��� " << this->index
			<< "������ " << this->name
			<< "������ " << this->type
			<< "��ֵ " << this->value << std::endl;
	}
};

//LexTool.cpp

Automaton* merge(Automaton* a, Automaton* b);
Automaton* closure(Automaton* p);
Automaton* connect(Automaton* p, Automaton* q);
void printNFA(Automaton* p, bool i = false, std::map<int, std::string>* q = NULL);
edges getEdges(int state, std::map<uint64, std::bitset<MAXCH>>& edge);
void standardA(std::map<uint64, std::bitset<MAXCH>>& edge);

//LexMaster.cpp
void readRE(std::string path);
void GRC(std::string line);
void prepareLex();
void processLex();

//REtoNFA
Automaton* mergeMultiA(std::map<int, std::string>& finalStates);
Automaton* getNFA(std::string RE);
int REtoNFA(std::vector<int>RE);

//NFAtoDFA.cpp
Automaton* NFAtoDFA(Automaton* oldA, std::map<int, std::string>& finalStates, std::map<int, std::string>& newFinalStates);

// SimplifyDFA.cpp
Automaton* simplifyDFA(Automaton* oldA, std::map<int, std::string>& finalStates, std::map<int, std::string>& simplifiedFinalStates);
int getEndState(Automaton* a, int st, int ch);

// Scanner.cpp
void readCode(std::string path);
int readNext(NametabItem& p);
void prepareLex();

class nameTable {
	NametabItem& find(std::string);
	bool update(std::string, NametabItem&);
};