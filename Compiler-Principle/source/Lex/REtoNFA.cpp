#include "Lex.h"
#include <algorithm>
#include <fstream>
using namespace std;

//已识别的短语和自动机之间的对应关系
map<string, Automaton*>CompleteNFA;

//可识别字符的位图
bitset<256>recognizeCh;

//自动机序号，比unsigned char型的最大值要大
int aIndex = 0xFFFF;

//自动机占位符的集合
map<int, Automaton*>automatons;

//存储词法
vector<string>lexs;

//记录被用作标识符的自动机
unordered_set<string>signA;

extern int stateIndex;

// 生成可识别字符的位图
void GRC(string line)
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
	for (auto i : line) {
		recognizeCh.set(i);
	}
}

//从文件中读取词法
void readRE(string path)
{
	ifstream file(path);

	if (!file.is_open()) {
		throw exception("词法读取失败");
	}

	string line;
	getline(file, line);
	GRC(line);
	while (getline(file, line)) {
		lexs.push_back(line);
	}

	// 关闭文件
	file.close();
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

//由正规式生成NFA
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
				throw exception("未找到匹配的右花括号");
			}
			string p = RE.substr(i + 1, j - i - 1);
			auto q = CompleteNFA.find(p);
			if (q == CompleteNFA.end()) {
				throw exception("未找到匹配的短语");
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

//合并多个自动机
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

//加工词法
void processLex()
{
	for (auto i : lexs) {
		auto j = i.find_first_of('>');
		auto k = i.substr(j + 1, i.size() - j - 1);
		CompleteNFA.insert(pair<string, Automaton*>{i.substr(0, j),getNFA(k)});
	}
}