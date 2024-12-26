#include "Lex.h"
#include <fstream>
#include <sstream>
using namespace std;

extern bitset<MAXCH>recognizeCh;
// ���ֱ� 
extern vector<NametabItem> nametab;
extern vector<pair<Automaton*, map<int, string>>>DFAS;
// �洢�����ļ� 
extern vector<string> codes;

int nameIndex = 0;	// ������ţ���0��ʼ�� 
int currPos = 0;	// ��ǰ�ַ��±꼴��������0��ʼ�� 
int lineCount = 0;	// ���� ����0��ʼ��

// �����ֱ������Ƿ�����name���ҵ������±꣬���򷵻�-1 
int findName(string name) {
	for (int i = 0; i < nametab.size(); i++) {
		if (nametab[i].name == name) {
			return i;
		}
	}
	return -1;
}

// ÿ����һ��ʶ��һ���´�
// item: NametabItem --> ʶ�𵽵Ĵ� 
// ����ֵ: SYN_ERROR --> �﷨���� 
//		   NOT_FINISHED --> ��ȡδ��� 
//		   FINISHED --> ��ȡ��� 
int readnext(NametabItem& item,Automaton* dfa,map<int,string>& finalStates,string& error) {
	string newName = ""; // �г����Ĵ� 
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
		throw LexException("��ȡ�����ļ���Χ��");
	}
	
	int lineLength = codes[lineCount].length();
	while (currPos <= lineLength) {
		char ch = line[currPos];	// ���һ���ַ�line[lineLength]��'\0' 
		arrvState = getEndState(dfa, nowState, ch);	// ����Զ����Ƿ��ܽ�������ַ� 
		// ���ܽ��� 
		if (arrvState == -1) {
			if (nowState == dfa->begin) {
				if (ch == ' ') {
					currPos++;
					continue;
				}
				else if (!recognizeCh.test(ch)) {
					newName += ch;
					NametabItem nametabItem(-1, newName,newName, 0);
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
				// ��ǰ������̬ 
				if (currPos == lineLength) {
					// ���ж����ˣ�����һ�� 
					lineCount++, currPos = 0;
				}

				int res = findName(newName);	// ����Ƿ����´� 
				if (res == -1) {
					NametabItem nametabItem(nameIndex++, newName, it_fs->second, 0);
					item = nametabItem;
					return NOT_FINISHED;	// ��δ���� 
				}
				else {

					item = nametab[res];
					return NOT_FINISHED;	// ��δ���� 
				}
			}
			else {
				ostringstream oss;
				// ��ǰû����̬������ 
				if (line[currPos] == '\0') {
					oss << "��" << lineCount + 1 << "�е�" << currPos + 1 << "�У�"
						<< codes[lineCount] << " "
						<< "�﷨����ȱ���ַ���";
					error = oss.str();
				}
				else {
					oss << "��" << lineCount + 1 << "�е�" << currPos + 1 << "�У�"
						<< codes[lineCount] << " "
						<< "�﷨����\"" << line[currPos] << "\"" << endl;
					error = oss.str();
				}
				
				return SYN_ERROR;	// �˳��������﷨���� 
			}
		}
		else {
			// ���Խ��� 
			newName += ch;
			nowState = arrvState;
			currPos++;
		}
	}

	return FINISHED;	// �����ļ���ȡ��ϣ�lineCount == codes.size() 
}

// �Ӵ����ļ��ж�ȡ���� 
void readCode(string path) {
	ifstream file(path);
	if (!file.is_open()) {
		throw ios_base::failure("�����ļ���ȡʧ��");
	}

	string line;
	while (getline(file, line)) {
		codes.push_back(line);
		cout << line<<endl;
	}
	
	// �ر��ļ�
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

	string e;
	NametabItem a;
	vector<state>p;
	p.push_back(state{ nameIndex, currPos, lineCount,NametabItem(), 0 });
	for (auto& i : DFAS) {
		nameIndex = p[0].a;
		currPos = p[0].b;
		lineCount = p[0].c;
		int k = 0;
		k= readnext(a, i.first, i.second,e);
		if (k == NOT_FINISHED ) {
			p.push_back(state{ nameIndex, currPos, lineCount,a, k });
		}
		
	}
	
	if (p.size()==1) {
		cout << e;
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