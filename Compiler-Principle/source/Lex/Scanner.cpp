#include "Lex.h"
#include <fstream>
#include <sstream>
using namespace std;

extern bitset<MAXCH>recognizeCh;
// 名字表 
extern vector<NametabItem> nametab;
extern vector<pair<Automaton*, map<int, string>>>DFAS;
// 存储代码文件 
extern vector<string> codes;

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
int readnext(NametabItem& item,Automaton* dfa,map<int,string>& finalStates,string& error) {
	string newName = ""; // 切出来的词 
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
		throw runtime_error("读取超过文件范围！");
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
					item = nametabItem;
					return NOT_FINISHED;	// 还未结束 
				}
				else {

					item = nametab[res];
					return NOT_FINISHED;	// 还未结束 
				}
			}
			else {
				ostringstream oss;
				// 当前没到终态，报错 
				if (line[currPos] == '\0') {
					oss << "第" << lineCount + 1 << "行第" << currPos + 1 << "列："
						<< codes[lineCount] << " "
						<< "语法错误：缺少字符。";
					error = oss.str();
				}
				else {
					oss << "第" << lineCount + 1 << "行第" << currPos + 1 << "列："
						<< codes[lineCount] << " "
						<< "语法错误：\"" << line[currPos] << "\"" << endl;
					error = oss.str();
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