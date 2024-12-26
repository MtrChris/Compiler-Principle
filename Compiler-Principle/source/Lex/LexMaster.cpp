#include "Lex.h"

#include <fstream>

using namespace std;
string path;
string codeFilePath;

//ȫ��ͨ�ã������������Զ�����ظ�
//״̬���
int stateIndex = 0;

vector<pair<Automaton*, map<int, string>>>DFAS;

vector<string>lexs;

//��ʶ��Ķ�����Զ���֮��Ķ�Ӧ��ϵ
map<string, Automaton*>CompleteNFA;

//��ʶ���ַ���λͼ
bitset<MAXCH>recognizeCh;

//�Զ�����ţ���unsigned char�͵����ֵҪ��
int aIndex = 0xFFFF;

//�Զ���ռλ���ļ���
map<int, Automaton*>automatons;


//��¼��������ʶ�����Զ���
unordered_set<string>signA;

// ���ֱ� 
vector<NametabItem> nametab;

// �洢�����ļ� 
vector<string> codes;

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
		throw ios_base::failure("�ʷ���ȡʧ��");
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

//�ӹ��ʷ�
void processLex(vector<pair<Automaton*, map<int, string>>>&nfas)
{
	for (auto i : lexs) {
		if (i.length() != 0) {
			auto j = i.find_first_of('>');
			auto k = i.substr(j + 1, i.size() - j - 1);
			auto p = i.substr(0, j);
			if (p=="letter"||p=="num"||p=="nnum") {
				Automaton* a = new Automaton();
				a->begin = stateIndex++;
				a->end = stateIndex++;
				a->edges[EDGE(a->begin, a->end)];
				for (auto ch : k) {
					if (ch != '|') {
						a->edges[EDGE(a->begin, a->end)].set(ch);
					}
				}
				CompleteNFA.insert(pair<string, Automaton*>{p, a});
			}
			else {
				CompleteNFA.insert(pair<string, Automaton*>{p, getNFA(k)});
			}
		}
		else if(!CompleteNFA.empty()){
			map<int,string>q;
			auto p = mergeMultiA(q);
			nfas.push_back(pair < Automaton*, map<int, string>>{p, q});
			CompleteNFA.clear();
			stateIndex = 0;
			signA.clear();
			automatons.clear();
		}
	}
	if (!CompleteNFA.empty()) {
		map<int, string>q;
		auto p = mergeMultiA(q);
		nfas.push_back(pair < Automaton*, map<int, string>>{p, q});
	}
	CompleteNFA.clear();
	stateIndex = 0;
	signA.clear();
	automatons.clear();
}


void prepareLex()
{
	try {
		cout << "����ʷ�·��" << endl;
		cin >> path;
		cout << "�������·��" << endl;
		cin >> codeFilePath;
		readRE(path);
		vector<pair<Automaton*, map<int, string>>>nfas;
		processLex(nfas);
		for (auto& i : nfas) {
			
			auto b = map<int, string>();
			auto a = NFAtoDFA(i.first, i.second, b);
            printNFA(a, true, &b);
			DFAS.push_back(pair<Automaton*, map<int, string>>{a, b});
		}
		readCode(codeFilePath);
	}
	catch (exception e) {
		cout << e.what();
	}
}

int main()
{
	try {
		prepareLex();
		NametabItem a;
		int b = 0;
		while (1) {
			b = readNext(a);
			if (b == FINISHED || b == -1) {
				break;
			}
			cout << a.name << "  " << a.type << endl;
		}
		return 0;
	}
	catch (exception e) {
		cout << e.what();
	}
}