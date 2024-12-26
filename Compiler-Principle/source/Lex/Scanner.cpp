#include "Lex.h"
#include <fstream>
using namespace std;

extern Automaton* dfa;
extern  map<int, string> finalStates;
extern bitset<MAXCH>recognizeCh;
// 名字表 
vector<NametabItem> nametab;

// 存储代码文件 
vector<string> codes;

int nameIndex = 0;	// 名字序号（从0开始） 
int currPos = 0;	// 当前字符下标即列数（从0开始） 
int lineCount = 0;	// 行数 （从0开始）

// 在名字表里找是否已有name，找到返回下标，否则返回-1 
int findName(string name) {
	for (int i = 0; i < nametab.size(); i++) {
		if (nametab[i].name == name) {
			return i;
		}
	}
	return -1;
}

// 每调用一次识别一个新词
// item: NametabItem --> 识别到的词 
// 返回值: SYN_ERROR --> 语法错误 
//		   NOT_FINISHED --> 读取未完成 
//		   FINISHED --> 读取完成 
int readNext(NametabItem& item) {
	string newName = ""; // 切出来的词 

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
		char ch = line[currPos];	// 最后一个字符line[lineLength]是'\0' 
		arrvState = getEndState(dfa, nowState, ch);	// 检查自动机是否能接受这个字符 
		// 不能接受 
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
				// 当前到达终态 
				if (currPos == lineLength) {
					// 本行读完了，读下一行 
					lineCount++, currPos = 0;
				}

				int res = findName(newName);	// 检查是否是新词 
				if (res == -1) {
					NametabItem nametabItem(nameIndex++, newName, it_fs->second, 0);
					nametab.push_back(nametabItem);	// 名字表中插入新词 
					// 返回、输出新词 
					//cout << "识别新词："; nametabItem.print();
					item = nametabItem;
					return NOT_FINISHED;	// 还未结束 
				}
				else {
					// 返回、输出已有词 
					//cout << "识别已有词："; nametab[res].print();
					item = nametab[res];
					return NOT_FINISHED;	// 还未结束 
				}
			}
			else {

				// 当前没到终态，报错 
				if (line[currPos] == '\0') {
					throw LexException(FORMATEXCEPTION(lineCount + 1, currPos + 1, codes[lineCount], "缺少字符"));
				}
				else {
					throw LexException(FORMATEXCEPTION(lineCount + 1, currPos + 1, codes[lineCount], line[currPos]));
				}
				
				return SYN_ERROR;	// 退出并返回语法错误 
			}
		}
		else {
			// 可以接受 
			newName += ch;
			nowState = arrvState;
			currPos++;
		}
	}

	return FINISHED;	// 代码文件读取完毕，lineCount == codes.size() 
}

// 从代码文件中读取句子 
void readCode(string path) {
	ifstream file(path);
	if (!file.is_open()) {
		throw ios_base::failure("代码文件读取失败");
	}

	string line;
	while (getline(file, line)) {
		codes.push_back(line);
		cout << line<<endl;
	}
	
	// 关闭文件
	file.close();
}
