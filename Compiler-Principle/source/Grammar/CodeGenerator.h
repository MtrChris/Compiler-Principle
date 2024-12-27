#pragma once
#include <stack>
#include <string>
#include <vector>
#include <iostream>
#include "GrammarParser.h"

#define TYPE "TYPE"
#define ASSIGN "ASSIGN"
#define SYMBOL "SYMBOL"
#define RELOP "RELOP"
#define OPERATOR "OPERATOR"
#define COMPARATOR "COMPARATOR"
#define IFSTMT "IF"
#define LEFTBRACKET "LEFTBRACKET"
#define RIGHTBRACKET "RIGHTBRACKET"

class Quadruple
{
public:
	std::string op;
	std::string arg1;
	std::string arg2;
	std::string result;
	int resultQuad = 0;
	Quadruple(std::string _op, std::string _arg1, std::string _arg2, std::string _result);
	Quadruple(std::string _op, std::string _arg1, std::string _arg2, int _resultQuad);
};

class Expr
{
public:
	std::string place;
	int nextList;
	int trueList;
	int falseList;
	int quad;
	Expr(std::string _place = "");
};
class CodeGenerator
{
private:
	std::vector<Quadruple> intermediateCode;
	int nextQuad;

public:
	int tempCount = 0;
	void printcode();
	void emit(Quadruple q);
	void backpatch(int list, int quad);
	int merge(int list1, int list2);
	int merge(int list1, int list2, int list3);
	void GenerateCode(ProductionAlg prod, std::stack<int>& stateStack, std::stack<Expr*>& symbolStack);
};