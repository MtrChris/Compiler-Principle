#include "Lex.h"
#include <algorithm>
#include <string>
using namespace std;

extern map<string, Automaton*>CompleteNFA;

//自动机序号，比char型的最大值要大
int aIndex = 0xFFFF;
map<int, Automaton*>automatons;
Automaton* getNFA(string RE)
{
	vector<int>re;
	automatons.clear();
	for (int i = 0; i < RE.length(); i++) {
		if (i != '{') {
			re.push_back((int)RE[i]);
		}
		else {
			int j = i + 1;
			while(RE[j]!='}'&&j<RE.length()) {
				j++;
			}
			if (j >= RE.length()) {
				throw exception("未找到匹配的花括号");
			}
			string 
		}
	}
}


Automaton* REtoNFA(vector<int> RE)
{
	//处理括号
	for (;;) {
		auto p=find(RE.rbegin(), RE.rend(), '(');
		if (p != RE.rend()) {
			auto begin = p.base() - 1;
			auto end = find(begin, RE.end(), ')');
			if (end != RE.end()) {
				REtoNFA(vector<int>(begin, end));
			}
		}
		else
			break;
	}
}

