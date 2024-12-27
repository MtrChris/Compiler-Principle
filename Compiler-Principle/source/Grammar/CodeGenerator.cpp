#include "CodeGenerator.h"
#include "GrammarParser.h"
#include <cstring>
using namespace std;

Quadruple::Quadruple(string _op, string _arg1, string _arg2, string _result)
{
    op = _op;
    arg1 = _arg1;
    arg2 = _arg2;
    result = _result;
}

Quadruple::Quadruple(string _op, string _arg1, string _arg2, int _resultQuad)
{
    op = _op;
    arg1 = _arg1;
    arg2 = _arg2;
    resultQuad = _resultQuad;
}

Expr::Expr(string _place)
{
    place = _place;
    nextList = trueList = falseList = quad = 0;
}


void CodeGenerator::emit(Quadruple q)
{
    if (q.result == "")
        q.result = "$T" + to_string(q.resultQuad);
    intermediateCode.push_back(q);
    ++nextQuad;
}


void CodeGenerator::backpatch(int list, int quad)
{
    while (list != 0)
    {
        int newlist = intermediateCode[list].resultQuad;
        intermediateCode[list].result = quad;
        list = newlist;
    }
}

int CodeGenerator::merge(int list1, int list2)
{
    int listbegin = list1;
    while (intermediateCode[list1].resultQuad != 0)
        list1 = intermediateCode[list1].resultQuad;
    intermediateCode[list1].result = list2;
    return listbegin;
}

int CodeGenerator::merge(int list1, int list2, int list3)
{
    int listbegin = list1;
    while (intermediateCode[list1].resultQuad != 0)
        list1 = intermediateCode[list1].resultQuad;
    intermediateCode[list1].result = list2;
    while (intermediateCode[list2].resultQuad != 0)
        list2 = intermediateCode[list2].resultQuad;
    intermediateCode[list2].result = list3;
    return listbegin;
}

void CodeGenerator::printcode()
{
    for (int i = 0; i < intermediateCode.size(); ++i)
        cout << "#" << i << ":" << intermediateCode[i].op << " " << intermediateCode[i].arg1 << " " << intermediateCode[i].arg2 << " " << intermediateCode[i].result << endl;
}

void CodeGenerator::GenerateCode(ProductionAlg prod, stack<int>& stateStack, stack<Expr*>& symbolStack)
{
    vector<Expr*> symbolInfo;
    Expr* res = new Expr(symbolStack.top()->place);
    for (int i = 0; i < prod.getAlgLength(); ++i)
    {
        stateStack.pop();
        Expr* curSymbol = symbolStack.top();
        symbolStack.pop();
        symbolInfo.push_back(curSymbol);
    }
    for (int i = 0; i < prod.getAlgLength() / 2; ++i)
    {
        Expr* temp = symbolInfo[i];
        symbolInfo[i] = symbolInfo[prod.getAlgLength() - i - 1];
        symbolInfo[prod.getAlgLength() - i - 1] = temp;
    }

    for (int i = 0; i < prod.getAlgLength(); ++i)
    {
        AlgElement* curElem = prod.getAlgElement(i);
        if (!curElem->getIsTerminal())
        {
            continue;
        }
        TerminalElement* curTerminal = dynamic_cast<TerminalElement*>(curElem);
        if (curTerminal->getType() == TYPE)
        {
            Expr* typeE = symbolInfo[i];
            Expr* varE = symbolInfo[i + 1];
            // Expr* assignE = symbolInfo[i + 2];
            // Expr* expE = symbolInfo[i + 3];
            emit(Quadruple("define", typeE->place, "-", varE->place));
            res->place = "$A" + nextQuad;
        }
        else if (curTerminal->getType() == ASSIGN)
        {
            Expr* leftE = symbolInfo[i - 1];
            Expr* rightE = symbolInfo[i + 1];
            emit(Quadruple(symbolInfo[i]->place, rightE->place, "-", leftE->place));
            res->place = "$S" + nextQuad;
        }
        else if (curTerminal->getType() == OPERATOR)
        {
            Expr* leftE = symbolInfo[i - 1];
            Expr* rightE = symbolInfo[i + 1];
            res->place = "$T" + to_string(tempCount++);
            emit(Quadruple(symbolInfo[i]->place, leftE->place, rightE->place, res->place));
        }
        else if (curTerminal->getType() == RELOP)
        {
            res->trueList = nextQuad;
            res->falseList = nextQuad + 1;
            Expr* leftE = symbolInfo[i - 1];
            Expr* rightE = symbolInfo[i + 1];
            res->place = "$S" + nextQuad;
            emit(Quadruple("j" + symbolInfo[i]->place, leftE->place, rightE->place, 0));
            emit(Quadruple("j", "-", "-", 0));
        }
        //{
        //    Expr E, E1, E2, M;
        //    //E -> E1 or M E2
        //    backpatch(E1.falseList, M.quad);
        //    E.trueList = merge(E1.trueList, E2.trueList);
        //    E.falseList = E2.falseList;
        //    //E -> E1 and M E2
        //    backpatch(E1.trueList, M.quad);
        //    E.trueList = E2.trueList;
        //    E.falseList = merge(E1.falseList, E2.falseList);
        //    //E -> not E1
        //    E.trueList = E1.falseList;
        //    E.falseList = E1.trueList;
        //    //M->��
        //    M.quad = nextQuad;
        //}
        //if (RELOP)
        //{
        //    Expr E, E1, E2, M, op;
        //    E.trueList = nextQuad;
        //    E.falseList = nextQuad + 1;
        //    emit(Quadruple("j" + op.place, E1.place, E2.place, 0));
        //    emit(Quadruple("j", "-", "-", 0));
        //}
        // if (IFWORD)
        // {
        //    Expr S, E, S1, S2, E2, M, M1, M2, N;
        //    //S->if E then M S1
        //    backpatch(E.trueList, M.quad);
        //    S.nextList = merge(E.falseList, S1.nextList);
        //    //S->if E then M1 S1 N else M2 S2
        //    backpatch(E.trueList, M1.quad);
        //    backpatch(E.trueList, M2.quad);
        //    S.nextList = merge(F. N.nextList, S2.nextList);
        //    //M->��
        //    M.quad = nextQuad;
        //    //N->��
        //    N.nextList = nextQuad;
        //    emit(Quadruple("j", "-", "-", 0));
        // }
        // if (curTerminal->getType() == IFWORD) {

        // }

        // else {
        //     tempStack.push(E0);
        // }
    }
    symbolStack.push(res);
}