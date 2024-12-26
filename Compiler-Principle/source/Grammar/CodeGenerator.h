#include<map>
#include <stack>
#include <string>
#include <vector>
#include <cstring>
#include <iostream>

class Quadruple
{
private:
    std::string op;
    std::string arg1;
    std::string arg2;
    std::string result;
public:
    Quadruple(std::string op, std::string arg1, std::string arg2, std::string result);
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
    std::map<int, Quadruple> intermediateCode;
public:
    void emit(int& nextStat, Quadruple q);
    void GenerateCode(int& nextStat, ProductionAlg prod, std::stack<int>& stateStack, std::stack<AlgElement *>& symbolStack);
};