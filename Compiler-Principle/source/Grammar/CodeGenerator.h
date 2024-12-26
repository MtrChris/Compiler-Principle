#include <stack>
#include <string>
#include <vector>
#include <iostream>
#include "GrammarParser.h"

#define ASSIGN "assign"
#define SYMBOL "symbol"

class Quadruple
{
private:
	std::string op;
	std::string arg1;
	std::string arg2;
	std::string result;

public:
	Quadruple(std::string op, std::string arg1, std::string arg2, std::string result) {}
};

class Expr
{
public:
	std::string place = "";
	int nextList = -1;
	int trueList = -1;
	int falseList = -1;
	int quad = -1;
};
class CodeGenerator
{
private:
	std::vector<Quadruple> intermediateCode;
	int nextStat;

public:
	void emit(Quadruple q) {}
	void GenerateCode(ProductionAlg prod, std::stack<int>& stateStack, std::stack<Expr*>& symbolStack) {}
};