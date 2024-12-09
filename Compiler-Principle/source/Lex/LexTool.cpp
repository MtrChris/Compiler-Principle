#include "Lex.h"
using namespace std;

//全局通用，用完递增，永远不会重复
//状态序号
int stateIndex = 0;
map<char, int>dict;

void Automaton::edgeMerge(const map<uint64, bitset<256>>&p)
{
    for (auto& i : p) {
        this->edges[i.first] |=i.second;
    }
}

Automaton::Automaton(char p)
{
    this->begin = stateIndex++;
    this->end = stateIndex++;
    this->edges[EDGE(this->begin, this->end)].set(p);
}

Automaton::Automaton()
{
    this->begin = 0;
    this->end = 0;
    this->edges[EDGE(this->begin, this->end)].reset();
}


/*
* 自动机的并运算后删除子自动机
*/
Automaton* merge(vector<Automaton*>FAS)
{
    int begin=stateIndex++,end = stateIndex++;
    Automaton* p = new Automaton();
    p->begin = begin;
    p->end = end;
    for (auto i : FAS) {
        p->edges[EDGE(begin, end)].set(Epsilon);
        p->edges[EDGE(i->end, end)].set(Epsilon);
        p->edgeMerge(i->edges);
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
    p->edges[EDGE(begin, p->begin)].set(Epsilon);
    p->edges[EDGE(p->end,end)].set(Epsilon);
    p->edges[EDGE(p->end, p->begin)].set(Epsilon);
    p->begin = begin;
    p->end = end;
    return p;
}

/*
* 连接两个自动机
*/
Automaton* connect(Automaton* p,Automaton*q)
{
    p->edges[EDGE(p->end, q->begin)].set(Epsilon);
    p->end = q->end;
    p->edgeMerge(q->edges);
    delete q;
    return p;
}








