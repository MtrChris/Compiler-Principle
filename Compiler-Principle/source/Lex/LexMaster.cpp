#include "Lex.h"

#include <fstream>

using namespace std;
string path;
string codeFilePath;

//全局通用，用完递增，永远不会重复
//状态序号
int stateIndex = 0;

vector<pair<Automaton*, map<int, string>>>DFAS;

vector<string>lexs;

//已识别的短语和自动机之间的对应关系
map<string, Automaton*>CompleteNFA;

//可识别字符的位图
bitset<MAXCH>recognizeCh;

//自动机序号，比unsigned char型的最大值要大
int aIndex = 0xFFFF;

//自动机占位符的集合
map<int, Automaton*>automatons;


//记录被用作标识符的自动机
unordered_set<string>signA;

// 名字表 
vector<NametabItem> nametab;

// 存储代码文件 
vector<string> codes;

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
		throw ios_base::failure("词法读取失败");
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

//加工词法
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
		cout << "输入词法路径" << endl;
		cin >> path;
		cout << "输入代码路径" << endl;
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