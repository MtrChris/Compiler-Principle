#include "Lex.h"
#include <algorithm>
#include <fstream>
using namespace std;

extern map<string, Automaton*>CompleteNFA;


extern bitset<MAXCH>recognizeCh;

extern int aIndex;


extern map<int, Automaton*>automatons;

extern vector<string>lexs;

extern unordered_set<string>signA;

extern int stateIndex;



int REtoNFA(vector<int>RE)
{
	if (RE.size() == 0)
		return -1;

	for (;;) {
		auto p = find(RE.rbegin(), RE.rend(), '(');
		if (p != RE.rend()) {
			auto b = p.base() - 1;
			auto d = find(b, RE.end(), ')');
			if (d != RE.end()) {
				int i = REtoNFA(vector<int>(b + 1, d));
				auto a = RE.erase(b, d + 1);
				if (i != -1) {
					RE.insert(a, i);
				}
			}
			else {
				throw LexException("括号不匹配");
			}
		}
		else
			break;
	}
	for (;;) {
		auto p = find(RE.begin(), RE.end(), '*');
		if (p == RE.begin()) {
			throw LexException("未找到匹配的闭包");
		}
		else if (p == RE.end())
			break;
		else {
			auto q = closure(automatons[*(p - 1)]);
			automatons[aIndex] = q;
			//printNFA(q);
			auto a = RE.erase(p - 1, p + 1);
			RE.insert(a, aIndex++);

		}
	}
	for (;;) {
		if (RE.size() == 1)
			break;
		else {
			auto a = RE.begin(), b = a + 1;
			if (*b == '|') {
				auto c = b + 1;
				if (c == RE.end()) {
					throw LexException("或运算未找到后项");
				}
				else {
					auto p = merge(automatons[*a], automatons[*c]);
					automatons[aIndex] = p;
					//printNFA(p);
					auto d = RE.erase(a, c + 1);
					RE.insert(d, aIndex++);
				}
			}
			else {
				auto p = connect(automatons[*a], automatons[*b]);
				//printNFA(p);
				automatons[aIndex] = p;
				auto d = RE.erase(a, b + 1);
				RE.insert(d, aIndex++);
			}
		}
	}
	return RE[0];
}

Automaton* getNFA(string RE)
{
	aIndex = 0xFFFF;
	vector<int>re;
	automatons.clear();
	for (int i = 0; i < RE.length();) {
		if (RE[i] != '{') {
			if (recognizeCh[RE[i]]) {
				auto p = new Automaton(RE[i]);
				automatons.insert(pair<int, Automaton*>{aIndex, p});
				re.push_back(aIndex++);
			}
			else
				re.push_back((int)RE[i]);
		}
		else {
			int j = i + 1;
			while (RE[j] != '}' && j < RE.length()) {
				j++;
			}
			if (j >= RE.length()) {
				throw LexException("未找到匹配的右花括号");
			}
			string p = RE.substr(i + 1, j - i - 1);
			auto q = CompleteNFA.find(p);
			if (q == CompleteNFA.end()) {
				throw LexException("未找到匹配的短语：" + p);
			}
			RE.replace(i, j - i + 1, "");
			auto r = new Automaton(q->second);
			automatons.insert(pair<int, Automaton*>{aIndex, r});
			signA.insert(p);
			re.push_back(aIndex++);
			continue;
		}
		i++;
	}
	int i = REtoNFA(re);
	auto p = automatons[i];
	automatons.clear();
	aIndex = 0xFFFF;
	return p;
}

Automaton* mergeMultiA(map<int, string>& finalStates)
{
	auto p = new Automaton();
	int j = stateIndex++;
	p->begin = j;
	for (auto& i : CompleteNFA) {
		//printNFA(i.second);
		if (signA.find(i.first) != signA.end())
			continue;

		p->edges[EDGE(j, i.second->begin)].set(Epsilon);
		p->edgeMerge(i.second->edges);
		finalStates.insert(pair<int, string>{i.second->end, i.first});
		delete i.second;
	}
	return p;
}