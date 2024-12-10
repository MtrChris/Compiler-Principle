#include "Lex.h"
#include <algorithm>
using namespace std;

extern map<string, Automaton*>CompleteNFA;

//可识别字符的位图
bitset<256>recognizeCh;

//除a-z,A-Z,0-9外可被识别的字符
char readableCh[] = { '_' };

//自动机序号，比unsigned char型的最大值要大
int aIndex = 0xFFFF;

//自动机占位符的集合
map<int, Automaton*>automatons;

int REtoNFA(vector<int>RE);


// 生成可识别字符的位图
void GRC()
{
	for (char i = 'a'; i <= 'z'; i++) {
		recognizeCh.set(i);
	}
	for (char i = 'A'; i <= 'Z'; i++) {
		recognizeCh.set(i);
	}
	for (char i = '0'; i <= '9'; i++) {
		recognizeCh.set(i);
	}
	for (auto i : readableCh) {
		recognizeCh.set(i);
	}
}

//由正规式生成NFA
Automaton* getNFA(string RE)
{
	aIndex = 0xFFFF;
	vector<int>re;
	automatons.clear();
	for (int i = 0; i < RE.length();) {
		if (i != '{') {
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
	REtoNFA(re);
	auto p = automatons[re[0]];
	automatons.clear();
	aIndex = 0xFFFF;
	return p;
}

//getNFA的递归子程序
int REtoNFA(vector<int>RE)
{
	if (RE.size()==0)
		return -1;
	//处理括号
	for (;;) {
		auto p = find(RE.rbegin(), RE.rend(), '(');
		if (p != RE.rend()) {
			auto b = p.base()-1;
			auto d = find(b, RE.end(), ')');
			if (d != RE.end()) {
				int i=REtoNFA(vector<int>(b+1,d));
				auto a = RE.erase(b, d+1);
				if (i != -1) {
					RE.insert(a, i);
					//printNFA(automatons[i]);
				}
			}
			else
				throw exception("括号不匹配");
		}
		else
			break;
	}
	//处理闭包
	for (;;) {
		auto p = find(RE.begin(), RE.end(), '*');
		if (p == RE.begin()) {
			throw exception("为找到匹配的闭包");
		}
		else if (p == RE.end())
			break;
		else {
			auto q=closure(automatons[*(p - 1)]);
			automatons[aIndex] = q;
			//printNFA(q);
			auto a =RE.erase(p - 1, p+1);
			RE.insert(a, aIndex++);

		}
	}
	//处理连接
	for (;;) {
		if (RE.size() == 1)
			break;
		else {
			auto a = RE.begin(), b = a + 1;
			if (*b == '|') {
				auto c = b + 1;
				if (c == RE.end())
					throw exception("或运算未找到后项");
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

