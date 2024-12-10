#include "Lex.h"
#include <algorithm>
using namespace std;

extern map<string, Automaton*>CompleteNFA;

//��ʶ���ַ���λͼ
bitset<256>recognizeCh;

//��a-z,A-Z,0-9��ɱ�ʶ����ַ�
char readableCh[] = { '_' };

//�Զ�����ţ���unsigned char�͵����ֵҪ��
int aIndex = 0xFFFF;

//�Զ���ռλ���ļ���
map<int, Automaton*>automatons;

int REtoNFA(vector<int>RE);


// ���ɿ�ʶ���ַ���λͼ
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

//������ʽ����NFA
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
				throw exception("δ�ҵ�ƥ����һ�����");
			}
			string p = RE.substr(i + 1, j - i - 1);
			auto q = CompleteNFA.find(p);
			if (q == CompleteNFA.end()) {
				throw exception("δ�ҵ�ƥ��Ķ���");
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

