#include "Lex.h"
using namespace std;

//ȫ��ͨ�ã������������Զ�����ظ�
//״̬���
int stateIndex = 0;

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
    this->edges.clear();
}


// �Զ����Ĳ�����
Automaton* merge(Automaton* a,Automaton*b)
{
    int begin = stateIndex++, end = stateIndex++;
    a->edges[EDGE(begin, a->begin)].set(Epsilon);
    a->edges[EDGE(begin, b->begin)].set(Epsilon);
    a->edges[EDGE(a->end,end)].set(Epsilon);
    a->edges[EDGE(b->end, end)].set(Epsilon);
    a->edgeMerge(b->edges);
    delete b;
    a->begin = begin;
    a->end = end;
    return a;
}

//�����Զ����ıհ�
Automaton* closure(Automaton* p) 
{
    int begin =stateIndex++, end =stateIndex++;
    p->edges[EDGE(begin, p->begin)].set(Epsilon);
    p->edges[EDGE(p->end,end)].set(Epsilon);
    p->edges[EDGE(p->end, p->begin)].set(Epsilon);
    p->edges[EDGE(begin, end)].set(Epsilon);
    p->begin = begin;
    p->end = end;
    return p;
}


//���������Զ���
Automaton* connect(Automaton* p,Automaton*q)
{
    p->edges[EDGE(p->end, q->begin)].set(Epsilon);
    p->end = q->end;
    p->edgeMerge(q->edges);
    delete q;
    return p;
}

//��ӡ�Զ���
void printNFA(Automaton* p)
{
    cout << p->begin << endl;
    cout << p->end << endl;
    for (auto& a : p->edges) {
        cout << BEGIN(a.first) << "    " << END(a.first) << "   ";
        if (a.second.test(0)) {
            cout << '&';
        }
        for (int i = 1; i < a.second.size(); i++) {
            if (a.second.test(i))
                cout << (char)i;
        }
        cout << endl;
    }

}

//���ָ���������б�
edges getEdge(int state,map<uint64,bitset<256>>& edge)
{
    struct edges p;
    p.begin=edge.upper_bound((uint64)state << 32);
    p.end = edge.upper_bound((uint64)(state + 1) << 32);
    return p;
}




