#include "Lex.h"
#include <unordered_map>
using namespace std;

// 记录所有发生转移的字符 ASCII 码 
unordered_set<int> allChars;

// 记录所有状态 
unordered_set<int> allStates;

// 记录当前分成了哪几个集合 
vector<unordered_set<int>> splitSets;

// 记录状态所在的集合的下标 
vector<int> splitSetsIndex;

void getAllChars(Automaton* a) {
    for (auto& edge: a->edges) {
        for (int i = 0; i < MAXCH; i ++) {
            if (edge.second.test(i)) {
                allChars.insert(i);
            }
        }
    }
}

void getAllStates(Automaton* a) {
    for (auto& edge : a->edges) {
        allStates.insert(BEGIN(edge.first));
        allStates.insert(END(edge.first));
    }
}

// 找状态 st 接受字符 ch 转移到什么状态 
int getEndState(Automaton* a, int st, int ch) {
    auto e = getEdges(st, a->edges);
    for (auto i = e.begin; i != e.end;i++) {
        if (BEGIN(i->first) == st && i->second.test(ch)) {
            return END(i->first);
        }
    }
    return -1; // 有可能接受不了 ch 
}

// 重新记录所有状态所在的集合的下标 
void getSplitSetsIndex() {
    splitSetsIndex.clear();
    for (auto& state : allStates) {
        splitSetsIndex.push_back(0);
    }

    int length = splitSets.size();
    for (int i = 0; i < length; i ++) {
        for (auto& state : splitSets[i]) {
            splitSetsIndex[state] = i;
        }
    }
}

// 分离状态集合 s 
// 返回 true 代表 s 被分离了，否则表示未分离
bool split(Automaton* a, unordered_set<int>& s) {
    int sIndex = splitSetsIndex[*(s.begin())]; // s 所在的集合的下标 

    // 判断 ch 能否分离 s 
    // 遍历所有字符 ch 
    for (auto& ch : allChars) {
        unordered_set<int> endIndex; // 记录 state 接受 ch 转移到的集合下标 
        map<int, int> stateEndIndex; // 记录 state 和其接受 ch 转移到的集合下标 
        for (auto& state : s) {
            int endState = getEndState(a, state, ch);
            if (endState != -1) {
                endIndex.insert(splitSetsIndex[endState]);
                stateEndIndex.insert(pair<int, int>{state, splitSetsIndex[endState]});
            }
            else {
                endIndex.insert(sIndex); // 不能接受的状态记为转移到本集合 
                stateEndIndex.insert(pair<int, int>{state, sIndex});
            }
        }

        if (endIndex.size() > 1) {
            // 分离 s 
            vector<unordered_set<int>> newSets;
            for (auto& index : endIndex) {
                unordered_set<int> newSet;
                for (auto& m : stateEndIndex) {
                    if (m.second == index) {
                        newSet.insert(m.first);
                    }
                }
                newSets.push_back(newSet);
            }

            // splitSets 增加 newSets 
            for (auto& newSet : newSets) {
                splitSets.push_back(newSet);
            }

            // 重新记录状态所在的集合的下标 
            getSplitSetsIndex();

            return true; // s 被清空了，退出并返回 true  
        }
    }
    return false;
}

// 分成终态集和非终态集 
// 如果有多个不同含义的终态，也应分开 
void initSplitSets(const map<int, string>& finalStates) {
    unordered_set<int> nonFinalStatesSet; // 非终态集 
    for (const auto& state : allStates) {
        if (finalStates.find(state) == finalStates.end()) {
            nonFinalStatesSet.insert(state);
        }
    }
    splitSets.push_back(nonFinalStatesSet);
    
    // 反过来创建一个 map，方便插入 
    unordered_map<string, unordered_set<int>> groupedFinalStates;
    for (const auto& pair : finalStates) {
        int leftKey = pair.first;
        string rightValue = pair.second;
        groupedFinalStates[rightValue].insert(leftKey);
    }
    
    // 提取 unordered_set 插入到 splitSets 中 
    for (const auto& group : groupedFinalStates) {
        splitSets.push_back(group.second);
    }
}

Automaton* simplifyDFA(Automaton* oldA, map<int, string>& finalStates, map<int, string>& simplifiedFinalStates) {
    Automaton* newA = new Automaton();
    
    getAllChars(oldA);
    getAllStates(oldA);

    // 先分成非终态集和终态集 
    initSplitSets(finalStates);

    // 记录状态所在的集合的下标 
    getSplitSetsIndex();

    // 分离子集，直到 splitSets 不变为止 
    while (true) {
        int lengthBefore = splitSets.size();
        for (int i = 0; i < lengthBefore; i ++) { // 这里使用 auto& s 的话，s.clear() 后 s.size() 异常
            if (splitSets[i].size() > 1) {
                if (split(oldA, splitSets[i])) {
                    splitSets[i].clear(); // 被分离后应该清空 
                }
            }
        }
        if (splitSets.size() == lengthBefore) {
            break;
        }
    }

    // 去除空集并倒序编号 
    splitSets.erase(
        remove_if(
            splitSets.begin(),
            splitSets.end(),
            [](const unordered_set<int>& s) { return s.empty(); }
        ),
        splitSets.end()
    );
    reverse(splitSets.begin(), splitSets.end());
    getSplitSetsIndex();

    // 0 所在的集合是初态 
    auto splitSets_it = find_if(
        splitSets.begin(),
        splitSets.end(),
        [](const unordered_set<int>& s) { return s.find(0) != s.end(); }
    );
    if (splitSets_it != splitSets.end()) {
        int startStateIndex = distance(splitSets.begin(), splitSets_it);
        newA->begin = startStateIndex;
    } else {
        cout << "Error: no start state (" << 0 <<") found." << endl;
    }

    // 原终态所在的集合是终态 
    for (auto& state : finalStates) {
        int endState = state.first;
        auto it = find_if(
            splitSets.begin(),
            splitSets.end(),
            [endState](const unordered_set<int>& s) { return s.find(endState) != s.end(); }
        );
        if (it != splitSets.end()) {
            int endStateIndex = distance(splitSets.begin(), it);
            simplifiedFinalStates.insert(pair<int, string>{endStateIndex, state.second});
        } else {
            cout << "Error: no end state (" << endState << ") found." << endl;
        }
    }

    // 构造新的 DFA 
    for (auto& oldEdge : oldA->edges) {
        uint64 st_ed = EDGE(
            splitSetsIndex[BEGIN(oldEdge.first)],
            splitSetsIndex[END(oldEdge.first)]
        );
        bitset<MAXCH> bs = oldEdge.second;

        // 如已有这条边，则合并 bitset，否则添加此边 
        // 这里的合并是指将两个 bitset 按位或，即两个边的字符集合的并集 
        auto it = newA->edges.find(st_ed);
        if (it != newA->edges.end()) {
            it->second |= bs;
        } else {
            newA->edges.insert(pair<uint64,bitset<MAXCH>>{st_ed, bs});
        }
    }

    delete oldA;
    return newA;
}