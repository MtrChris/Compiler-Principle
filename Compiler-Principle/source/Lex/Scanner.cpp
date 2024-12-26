#include "Lex.h"
#include <fstream>
#include <sstream>
using namespace std;

extern bitset<MAXCH>recognizeCh;

extern vector<NametabItem> nametab;
extern vector<pair<Automaton*, map<int, string>>>DFAS;
extern vector<string> codes;

int nameIndex = 0;	
int currPos = 0;	
int lineCount = 0;	
 
int findName(string name) {
	for (int i = 0; i < nametab.size(); i++) {
		if (nametab[i].name == name) {
			return i;
		}
	}
	return -1;
}


int readnext(NametabItem& item, Automaton* dfa, map<int, string>& finalStates, LexException error) {
	string newName = ""; 
	//printNFA(dfa);
	int beginState = dfa->begin;
	int nowState = beginState;
	int arrvState;

	string line;
	if (lineCount < codes.size()) {
		line = codes[lineCount];
	}
	else if (lineCount == codes.size()) {
		return FINISHED;
	}
	else {
		throw LexException("读取超过文件范围！");
	}

	int lineLength = codes[lineCount].length();
	while (currPos <= lineLength) {
		char ch = line[currPos]; 
		arrvState = getEndState(dfa, nowState, ch);	

		if (arrvState == -1) {
			if (nowState == dfa->begin) {
				if (ch == ' ') {
					currPos++;
					continue;
				}
				else if (!recognizeCh.test(ch)) {
					newName += ch;
					NametabItem nametabItem(-1, newName, newName, 0);
					item = nametabItem;
					currPos++;
					if (currPos == lineLength) {
						lineCount++, currPos = 0;
					}
					return NOT_FINISHED;
				}
			}
			auto it_fs = finalStates.find(nowState);
			if (it_fs != finalStates.end()) {
				if (currPos == lineLength) {
					lineCount++, currPos = 0;
				}

				int res = findName(newName);	
				if (res == -1) {
					NametabItem nametabItem(nameIndex++, newName, it_fs->second, 0);
					item = nametabItem;
					return NOT_FINISHED;	
				}
				else {

					item = nametab[res];
					return NOT_FINISHED;	
				}
			}
			else {
				ostringstream oss;
				if (line[currPos] == '\0') {
				    error= LexException(FORMATEXCEPTION(lineCount + 1, currPos + 1, codes[lineCount], "缺少字符"));
				}
				else {
					error= LexException(FORMATEXCEPTION(lineCount + 1, currPos + 1, codes[lineCount], line[currPos]));
				}

				return SYN_ERROR;	
			}
		}
		else {
			newName += ch;
			nowState = arrvState;
			currPos++;
		}
	}

	return FINISHED;	
}

void readCode(string path) {
	ifstream file(path);
	if (!file.is_open()) {
		throw ios_base::failure("代码文件读取失败");
	}

	string line;
	while (getline(file, line)) {
		codes.push_back(line);
		cout << line << endl;
	}

	file.close();
}

struct state {
	int a;
	int b;
	int c;
	NametabItem p;
	int r;
};

int readNext(NametabItem& item)
{

	LexException e;
	NametabItem a;
	vector<state>p;
	p.push_back(state{ nameIndex, currPos, lineCount,NametabItem(), 0 });
	bool j = false;
	for (auto& i : DFAS) {
		nameIndex = p[0].a;
		currPos = p[0].b;
		lineCount = p[0].c;
		int k = readnext(a, i.first, i.second, e);
		if (k == NOT_FINISHED) {
			p.push_back(state{ nameIndex, currPos, lineCount,a, k });
		}
		else if (k == FINISHED) {
			j = true;
		}

	}
	if (j) {
		return FINISHED;
	}
    else if (p.size()==1) {
		throw e;
		return SYN_ERROR;
	}
	auto it = max_element(p.begin(), p.end(), [](state a, state b) {
		if (a.c < b.c)
			return true;
		else if (a.c == b.c)
			return a.b < b.b;
		else
			return false;
		});
	item = it->p;
	nameIndex = it->a;
	currPos = it->b;
	lineCount = it->c;
	nametab.push_back(item);
	return it->r;
}