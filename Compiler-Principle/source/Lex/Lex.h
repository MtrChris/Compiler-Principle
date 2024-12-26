#pragma once
#include <vector>
#include <map>
#include <bitset>
#include <string>
#include <iostream>
#include <unordered_set>
#include <unordered_map>
#include <algorithm>
#include <exception>

#define Epsilon 0
#define MAXCH 256
#define SYN_ERROR -1
#define NOT_FINISHED 0
#define FINISHED 1

using uint64 = unsigned long long int;
//取起点
#define BEGIN(x) ((int)((uint64)(x)>>32)) 
//起点和终点合成边
#define EDGE(x,y) (((uint64)(x)<<32)+(uint64)(y))
//取终点
#define END(x) ((int)(x))
//将边x的起点和终点同时偏移y
#define OFFSET(x,y) ((uint64)(x)+((uint64)(y)<<32)+y)

//报错提示
#define FORMATEXCEPTION(line, col, code, ch) \
	"第" + to_string(line) + "行第" + to_string(col) + "列" + \
	(code) + " 词法分析错误：\"" + (ch) + "\""\

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
	int index;				// 序号，从0开始 
	std::string name;		// 名字，如abc 
	std::string type;		// 类型，如123>abc的123 
	int value;				// 值，默认是整数0，根据需要修改
	NametabItem() {};
	NametabItem(int index, std::string name, std::string type, int value) {
		this->index = index;
		this->name = name;
		this->type = type;
		this->value = value;
	}
	void print() {
		std::cout << "序号 " << this->index
			<< "，名字 " << this->name
			<< "，类型 " << this->type
			<< "，值 " << this->value << std::endl;
	}
};

//LexTool.cpp

Automaton* merge(Automaton* a, Automaton* b);
Automaton* closure(Automaton* p);
Automaton* connect(Automaton* p, Automaton* q);
void printNFA(Automaton* p, bool i = false, std::map<int, std::string>* q = NULL);
edges getEdges(int state, std::map<uint64, std::bitset<MAXCH>>& edge);
void standardA(std::map<uint64, std::bitset<MAXCH>>& edge);

//REtoNFA

void readRE(std::string path);
void processLex();
Automaton* mergeMultiA(std::map<int, std::string>& finalStates);

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

class LexException: public std::exception
{
  std::string _msg;

public:
  LexException(const std::string &msg = "词法分析错误");
  const char *what();
};