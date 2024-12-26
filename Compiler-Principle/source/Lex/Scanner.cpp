#include "Lex.h"
#include <fstream>
using namespace std;

extern Automaton* dfa;
extern  map<int, string> finalStates;
extern bitset<MAXCH>recognizeCh;
// ���ֱ� 
vector<NametabItem> nametab;

// �洢�����ļ� 
vector<string> codes;

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
int readNext(NametabItem& item) {
	string newName = ""; // �г����Ĵ� 

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
					nametab.push_back(nametabItem);	// ���ֱ��в����´� 
					// ���ء�����´� 
					//cout << "ʶ���´ʣ�"; nametabItem.print();
					item = nametabItem;
					return NOT_FINISHED;	// ��δ���� 
				}
				else {
					// ���ء�������д� 
					//cout << "ʶ�����дʣ�"; nametab[res].print();
					item = nametab[res];
					return NOT_FINISHED;	// ��δ���� 
				}
			}
			else {

				// ��ǰû����̬������ 
				if (line[currPos] == '\0') {
					throw LexException(FORMATEXCEPTION(lineCount + 1, currPos + 1, codes[lineCount], "ȱ���ַ�"));
				}
				else {
					throw LexException(FORMATEXCEPTION(lineCount + 1, currPos + 1, codes[lineCount], line[currPos]));
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
