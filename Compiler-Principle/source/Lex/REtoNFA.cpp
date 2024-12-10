#include "Lex.h"
#include <algorithm>
#include <fstream>
using namespace std;

//��ʶ��Ķ�����Զ���֮��Ķ�Ӧ��ϵ
map<string, Automaton*>CompleteNFA;

//��ʶ���ַ���λͼ
bitset<256>recognizeCh;

//�Զ�����ţ���unsigned char�͵����ֵҪ��
int aIndex = 0xFFFF;

//�Զ���ռλ���ļ���
map<int, Automaton*>automatons;

//�洢�ʷ�
vector<string>lexs;

//��¼��������ʶ�����Զ���
unordered_set<string>signA;

extern int stateIndex;

// ���ɿ�ʶ���ַ���λͼ
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

//���ļ��ж�ȡ�ʷ�
void readRE(string path)
{
	ifstream file(path);

	if (!file.is_open()) {
		throw exception("�ʷ���ȡʧ��");
	}

	string line;
	getline(file, line);
	GRC(line);
	while (getline(file, line)) {
		lexs.push_back(line);
	}

	// �ر��ļ�
	file.close();
}


//getNFA�ĵݹ��ӳ���
int REtoNFA(vector<int>RE)
{
	if (RE.size()==0)
		return -1;
	//��������
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
				throw exception("���Ų�ƥ��");
		}
		else
			break;
	}
	//����հ�
	for (;;) {
		auto p = find(RE.begin(), RE.end(), '*');
		if (p == RE.begin()) {
			throw exception("Ϊ�ҵ�ƥ��ıհ�");
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
	//��������
	for (;;) {
		if (RE.size() == 1)
			break;
		else {
			auto a = RE.begin(), b = a + 1;
			if (*b == '|') {
				auto c = b + 1;
				if (c == RE.end())
					throw exception("������δ�ҵ�����");
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

//������ʽ����NFA
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
				throw exception("δ�ҵ�ƥ����һ�����");
			}
			string p = RE.substr(i + 1, j - i - 1);
			auto q = CompleteNFA.find(p);
			if (q == CompleteNFA.end()) {
				throw exception("δ�ҵ�ƥ��Ķ���");
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

//�ϲ�����Զ���
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

//�ӹ��ʷ�
void processLex()
{
	for (auto i : lexs) {
		auto j = i.find_first_of('>');
		auto k = i.substr(j + 1, i.size() - j - 1);
		CompleteNFA.insert(pair<string, Automaton*>{i.substr(0, j),getNFA(k)});
	}
}