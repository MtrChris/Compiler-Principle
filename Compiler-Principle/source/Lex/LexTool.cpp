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

Automaton::Automaton(const Automaton* other)
{
    int min = INT_MAX,a=0,b=0;
    unordered_set<int> s;
    for (auto& p : other->edges) {
        a = BEGIN(p.first);
        b = END(p.first);
        min =a < min ? a : min;
        min =b< min ? b : min;
        if (s.find(a) == s.end()) {
            s.insert(a);
        }
        if (s.find(b) == s.end()) {
            s.insert(b);
        }
    }
    int c = stateIndex - min;
    stateIndex += s.size();
    this->begin = other->begin + c;
    this->end = other->end + c;
    for (auto& p : other->edges) {
        this->edges.insert(pair<uint64, bitset<256>>{OFFSET(p.first,c), p.second});
    }
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
void printNFA(Automaton* p,bool i ,map<int, string>* q)
{
    if (i) {
        cout << p->begin << endl;
        cout << p->end << endl;
    }
    for (auto& a : p->edges) {
        cout << BEGIN(a.first) << "    " << END(a.first) << "   ";
        if (a.second.test(0)) {
            cout << '&';
        }
        for (int i = 1; i < a.second.size(); i++) {
            if (a.second.test(i))
                cout << (char)i;
        }
        if (q != NULL) {
            auto k = q->find(END(a.first));
            if (k != q->end())
                cout << "  " << "��̬:" << k->second;
        }
        cout << endl;
    }

}

//���ָ���������б�
edges getEdges(int state,map<uint64,bitset<256>>& edge)
{
    struct edges p;
    p.begin=edge.lower_bound((uint64)state << 32);
    p.end = edge.lower_bound((uint64)(state + 1) << 32);
    return p;
}

void standardA(map<uint64, bitset<256>>& edge)
{
    for (auto& i : edge) {
        if (i.second.count() > 1) {
            i.second.reset(Epsilon);
        }
    }
}


