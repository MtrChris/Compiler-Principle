#include "Lex.h"
#include <algorithm>
#include <string>
using namespace std;

extern map<string, Automaton*>CompleteNFA;
extern int stateIndex;
extern bitset<256>recognizeCh;

//自动机序号，比unsigned char型的最大值要大
int aIndex = 0xFFFF;
map<int, Automaton*>automatons;


Automaton* getNFA(string RE)
{
	aIndex = 0xFFFF;
	vector<int>re;
	automatons.clear();
	for (int i = 0; i < RE.length();) {
		if (i != '{') {
			if (recognizeCh[RE[i]]) {
				auto p = new Automaton(i);
				automatons.insert(pair<int, Automaton*>{aIndex, p});
				re.push_back(aIndex++);
			}
			else
				re.push_back((int)i);
		}
		else {
			int j = i + 1;
			while(RE[j]!='}'&&j<RE.length()) {
				j++;
			}
			if (j >= RE.length()) {
				throw exception("未找到匹配的右花括号");
			}
			string p = RE.substr(i + 1, j - i - 1);
			auto q = CompleteNFA.find(p);
			if (q == CompleteNFA.end()) {
				throw exception("未找到匹配的短语");
			}
			RE.replace(i, j - i + 1, "");
			automatons.insert(pair<int, Automaton*>{aIndex, q->second});
			re.push_back(aIndex++);
			continue;
		}
		i++;
	}
	REtoNFA(re.begin(),--re.end());
	auto p = automatons[re[0]];
	automatons.clear();
	aIndex = 0xFFFF;
	return p;
}


void REtoNFA(vector<int>::iterator begin, vector<int>::iterator end)
{
	if (end == begin)
		return;
	//处理括号
	for (;;) {
		auto p=find(make_reverse_iterator(end), make_reverse_iterator(begin), '(');
		if (p != make_reverse_iterator(begin)) {
			auto b = p.base();
			auto d = find(b, end, ')');
			if (d != end) {
				REtoNFA(b,d);
			}
			else
				throw exception("括号不匹配");
		}
		else
			break;
	}
}

