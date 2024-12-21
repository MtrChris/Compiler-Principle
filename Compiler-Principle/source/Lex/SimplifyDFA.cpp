#include "Lex.h"
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
    for (auto& edge : a->edges) {
        if (BEGIN(edge.first) == st && edge.second.test(ch)) {
            return END(edge.first);
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
                endIndex.insert(-1); // 不能接受的状态记为 -1 
                stateEndIndex.insert(pair<int, int>{state, -1});
            }
        }

        /*****************************************************************
        *  对“不接受”的状态进行处理： 
        *  1. 如果均为“不接受”的状态，不进行处理； 
        *  2. 否则，进行以下处理：
        *     a. 如果有转移到本集合的状态，-1 优先改为本集合下标（视为转移到本集合） 
        *     b. 如果没有， - 1 将改为首个非 - 1 的下标（视为转移到此集合） 
        ******************************************************************/
        if (endIndex.size() > 1 && endIndex.find(-1) != endIndex.end()) {
            endIndex.erase(-1);
            if (endIndex.find(sIndex) != endIndex.end()) {
                for (auto& m : stateEndIndex) {
                    if (m.second == -1)
                        m.second = sIndex;
                }
            }
            else {
                for (auto& m : stateEndIndex) {
                    if (m.second == -1)
                        m.second = *(endIndex.begin());
                }
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

Automaton* simplifyDFA(Automaton* oldA, map<int, string>& finalStates, map<int, string>& simplifiedFinalStates) {
    Automaton* newA = new Automaton();
    
    getAllChars(oldA);
    getAllStates(oldA);

    // 先分成非终态集和终态集 
    unordered_set<int> nonFinalStatesSet;
    unordered_set<int> finalStatesSet;
    for (auto& state : allStates) {
        if (finalStates.find(state) != finalStates.end())
            finalStatesSet.insert(state);
        else
            nonFinalStatesSet.insert(state);
    }
    splitSets.push_back(nonFinalStatesSet); // 下标 0 
    splitSets.push_back(finalStatesSet);    // 下标 1 

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
            newA->end = endStateIndex; // 多个终态时，会出错 
            //cout << "end state: " << endStateIndex << endl; // 测试一下 
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