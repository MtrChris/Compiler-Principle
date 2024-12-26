#include "CodeGenerator.h"
#include "GrammarParser.h"
using namespace std;

Quadruple::Quadruple(std::string o, std::string a1, std::string a2, std::string r)
{
    op = o;
    arg1 = a1;
    arg2 = a2;
    result = r;
}

void CodeGenerator::emit(int &nextStat, Quadruple q)
{
    intermediateCode[nextStat++] = q;
}

void CodeGenerator::GenerateCode(int& nextStat, ProductionAlg prod, stack<int> &stateStack, stack<AlgElement *> &symbolStack)
{
    vector<AlgElement> temp;
    TerminalElement* nowAlg, * lastAlg, * nextAlg;
    Expr* E;
    for (int i = 0; i < prod.rightAlg.size(); ++i) 
    {
        stateStack.pop();     
        if (symbolStack.top()->getIsTerminal())
        {
            nowAlg = dynamic_cast<TerminalElement*>(symbolStack.top());
            symbolStack.pop();
            if (nowAlg->getType() == "operator")
            {
                nextAlg = dynamic_cast<TerminalElement*>(symbolStack.top());
                symbolStack.pop();
                NonTerminalElement* temp = new NonTerminalElement("$T" + to_string(nextStat));
                symbolStack.push(temp);
                Quadruple q = Quadruple(nowAlg->getVal(), nextAlg->getVal(), lastAlg->getVal(), temp->getName());
                emit(nextStat, q);
            }

            if (nowAlg->getType() == "assignment")
            {
                nextAlg = dynamic_cast<TerminalElement*>(symbolStack.top());
                symbolStack.pop();
                Quadruple q = Quadruple(nowAlg->getVal(), "-", lastAlg->getVal(), nextAlg->getVal());
                emit(nextStat, q);
            }

            if (nowAlg->getType() == "relop")
            {
                nextAlg = dynamic_cast<TerminalElement*>(symbolStack.top());

            }
        }
    }

}