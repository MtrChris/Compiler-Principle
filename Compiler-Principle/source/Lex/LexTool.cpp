#include "Lex.h"
using namespace std;

//全局通用，用完递增，永远不会重复
//状态序号
int stateIndex = 0;


/*
* 自动机的并运算后删除子自动机
*/
Automaton* merge(vector<Automaton*>FAS)
{
    int begin=stateIndex++,end = stateIndex++;
    Automaton* p = new Automaton;
    p->begin = begin;
    p->end = end;
    for (auto i : FAS) {
        p->edges.push_back(edge( begin, i->begin, Epsilon));
        p->edges.push_back(edge(i->end, end, Epsilon));
        p->edges.insert(p->edges.end(),i->edges.begin(),i->edges.end());
        delete i;
    }
    return p;
}

/*
* 生成自动机的闭包
*/
Automaton* closure(Automaton* p) 
{
    int begin =stateIndex++, end =stateIndex++;
    p->edges.push_back(edge(begin, p->begin, Epsilon));
    p->edges.push_back(edge(p->end, end, Epsilon));
    p->edges.push_back(edge(p->end, p->begin, Epsilon));
    p->begin = begin;
    p->end = end;
    return p;
}

/*
* 连接两个自动机
*/
Automaton* connect(Automaton* p,Automaton*q)
{
    p->edges.push_back(edge(p->end, q->begin, Epsilon));
    p->end = q->end;
    p->edges.insert(p->edges.end(), q->edges.begin(), q->edges.end());
    delete q;
    return p;
}








