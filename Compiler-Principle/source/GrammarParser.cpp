#include "GrammarParser.h"
#include "InputHandler.h"
#include <queue>
// #include <algorithm> // sort
using namespace std;

// -------- AlgElement类的成员函数实现 --------
AlgElement::AlgElement() : isTerminal(false), id(-1)
{
}

bool AlgElement::getIsTerminal() const
{
  return isTerminal;
}

int AlgElement::getId() const
{
  return id;
}

bool AlgElement::operator==(const AlgElement &right)
{
  return isTerminal == right.getIsTerminal() && id == right.getId() && id != -1;
}

// -------- ElementDict类的成员函数实现 ---------
TerminalElement *const ElementDict::CANDIDATEMARK = new TerminalElement("$CANDIDATEMARK$");
TerminalElement *const ElementDict::EMPTYWORD = new TerminalElement();
TerminalElement *const ElementDict::ENDCH = new TerminalElement("$ENDCH$");
NonTerminalElement *const ElementDict::STARTSYMBOL = new NonTerminalElement("$START$");

void ElementDict::init(GrammarParser *_parser)
{
  elemList.clear();
  parser = _parser;
  createElem(CANDIDATEMARK);
  createElem(EMPTYWORD);
  createElem(ENDCH);
  createElem(STARTSYMBOL);
}

void ElementDict::createElem(AlgElement *elem)
{
  elemList.push_back(elem);
  elem->id = elemList.size() - 1;
}

int ElementDict::findElem(const AlgElement &elem)
{
  for (int i = 0; i < elemList.size(); i++)
  {
    if (*elemList[i] == elem)
    {
      return i;
    }
  }
  return -1;
}

int ElementDict::findElem(const string &elemName)
{
  return findElem(NonTerminalElement(elemName));
}

void ElementDict::updateElem(AlgElement *&elem)
{
  int findRes = findElem(*elem);
  if (findRes == -1)
  {
    createElem(elem);
  }
  else
  {
    if (elem->id == -1)
    {
      delete elem;
    }
    elem = elemList[findRes];
  }
}

AlgElement *ElementDict::getElem(int index)
{
  return elemList[index];
}

void ElementDict::classifyAlg()
{
  // 将产生式按照非终结符分类
  parser->classifiedAlgs.resize(elemList.size());
  for (int i = 0; i < parser->algs.size(); i++)
  {
    parser->classifiedAlgs[parser->algs[i].leftElem->id].push_back(parser->algs[i].id);
  }
}

void ElementDict::calculateFirst()
{
  firstSet.resize(elemList.size());
  for (int i = 2; i < elemList.size(); i++)
  {
    if (!elemList[i]->isTerminal)
    {
      vector<bool> calcState(elemList.size(), false);
      for (int j = 2; j < i; j++)
      {
        calcState[j] = true;
      }
      _calculateFirst(i, calcState);
    }
  }
  // for (int i = 2; i < elemList.size(); i++)
  // {
  //   elemList[i]->print();
  //   cout << "FIRST(" << i << ") = ";
  //   for (set<int>::iterator it = firstSet[i].begin(); it != firstSet[i].end(); it++)
  //   {
  //     cout << *it << " ";
  //   }
  //   cout << endl;
  // }
}

void ElementDict::_calculateFirst(int curElem, vector<bool> &calcState)
{
  calcState[curElem] = true;
  for (int i = 0; i < parser->classifiedAlgs[curElem].size(); i++)
  {
    ProductionAlg &curAlg = parser->algs[parser->classifiedAlgs[curElem][i]];
    // 整个产生式为空字时
    if (curAlg.rightAlg.size() == 0 || curAlg.rightAlg.size() == 1 && *curAlg.rightAlg[0] == *ElementDict::EMPTYWORD)
    {
      firstSet[curElem].insert(EMPTYWORDID);
      continue;
    }
    int j = 0;
    for (; j < curAlg.rightAlg.size(); j++)
    {
      int algElemId = curAlg.rightAlg[j]->id;
      // 1. 遇到终结符：直接加入FIRST集合
      if (curAlg.rightAlg[j]->isTerminal)
      {
        firstSet[curElem].insert(algElemId);
        break;
      }
      // 2. 遇到非终结符：若未计算则计算FIRST
      else if (!calcState[algElemId])
      {
        _calculateFirst(algElemId, calcState);
      }
      // 将该非终结符的所有FIRST集合（除了空字）加入
      for (set<int>::iterator it = firstSet[algElemId].begin(); it != firstSet[algElemId].end(); it++)
      {
        if (*it != 1)
        {
          firstSet[curElem].insert(*it);
        }
      }
      // 该非终结符不包含空集时，查找下一个产生式
      if (firstSet[algElemId].find(EMPTYWORDID) == firstSet[algElemId].end())
      {
        break;
      }
    }
    if (j == curAlg.rightAlg.size())
    {
      firstSet[curElem].insert(EMPTYWORDID); // 读完了整个产生式，表示可以推导为空字
    }
  }
}

const set<int> &ElementDict::getFirstSet(int id)
{
  return firstSet[id];
}

ElementDict::~ElementDict()
{
  for (int i = 0; i < elemList.size(); i++)
  {
    delete elemList[i];
  }
}

// -------- TerminalElement类的成员函数实现  --------

TerminalElement::TerminalElement(string _type, string _val) : type(_type), val(_val)
{
  isTerminal = true;
}

bool TerminalElement::operator==(const AlgElement &right)
{
  const TerminalElement *tRight = dynamic_cast<const TerminalElement *>(&right);
  if (!tRight)
  {
    return false;
  }
  return type == tRight->type && val == tRight->val;
}

// -------- NonTerminalElement类的成员函数实现 --------

NonTerminalElement::NonTerminalElement(string _name) : name(_name)
{
  isTerminal = false;
}

bool NonTerminalElement::operator==(const AlgElement &right)
{
  const NonTerminalElement *nRight = dynamic_cast<const NonTerminalElement *>(&right);
  if (!nRight)
  {
    return false;
  }
  return name == nRight->name;
}

// -------- ProductionAlg类的成员函数实现 --------
ProductionAlg::ProductionAlg(AlgElement *_leftElem) : leftElem(_leftElem)
{
}

ProductionAlg::ProductionAlg(AlgElement *_leftElem, vector<AlgElement *> &_rightAlg) : leftElem(_leftElem), rightAlg(_rightAlg)
{
}

void ProductionAlg::readFromStr(const string &s, ElementDict &dict)
{
  string word;
  InputReader reader(s);
  // 读取左边的元素
  reader.getNextWord(word);
  leftElem = new NonTerminalElement(word);
  dict.updateElem(leftElem);
  // 检查定义格式
  reader.getNextWord(word);
  if (word != "->")
  {
    throw InputException("产生式定义错误");
  }
  // 读取右边的元素
  while (reader.hasNextWord())
  {
    AlgElement *elem = nullptr;
    reader.getNextWord(word);
    if (word == "{")
    {
      // 对终结符进行读取
      int readcnt = 0;
      string terminalInfo[2];
      reader.getNextWord(word);
      for (int readcnt = 0; word != "}" && readcnt < 2; readcnt++)
      {
        terminalInfo[readcnt] = word;
        InputReader::removeEscape(terminalInfo[readcnt]);
        reader.getNextWord(word);
      }
      if (word != "}")
      {
        throw InputException("终结符输入格式错误");
      }
      elem = new TerminalElement(terminalInfo[0], terminalInfo[1]);
    }
    else if (word == "|")
    {
      elem = ElementDict::CANDIDATEMARK;
    }
    else
    {
      elem = new NonTerminalElement(word);
    }
    dict.updateElem(elem);
    rightAlg.push_back(elem);
  }
}

void ProductionAlg::splitAlg(vector<ProductionAlg> &algs)
{
  int srcSize = algs.size();
  vector<AlgElement *> startElem(1, algs[0].leftElem);
  ProductionAlg startAlg(ElementDict::STARTSYMBOL, startElem);
  startAlg.id = 0;
  algs.push_back(startAlg);
  for (int i = 0; i < srcSize; i++)
  {
    vector<AlgElement *>::iterator partStart = algs[i].rightAlg.begin();
    vector<AlgElement *>::iterator partEnd = algs[i].rightAlg.begin();
    // 以候选式符拆分右侧产生式
    while (partStart != algs[i].rightAlg.end())
    {
      ProductionAlg newAlg(algs[i].leftElem);
      for (; partEnd != algs[i].rightAlg.end() && !(**partEnd == *ElementDict::CANDIDATEMARK); partEnd++)
      {
        ;
      }
      for (; partStart != partEnd; partStart++)
      {
        newAlg.rightAlg.push_back(*partStart);
      }
      if (partEnd != algs[i].rightAlg.end())
      {
        ++partEnd;
      }
      partStart = partEnd;
      newAlg.id = algs.size() - srcSize;
      algs.push_back(newAlg);
    }
  }
  algs.erase(algs.begin(), algs.begin() + srcSize);
}

// -------- GrammarDFAAlg类的成员函数实现 --------
DFANodeAlg::DFANodeAlg(int _algId, int _curpos, std::set<int> _prospectCh)
    : algId(_algId), curpos(_curpos), prospectCh(_prospectCh)
{
}

DFANodeAlg DFANodeAlg::createNext() const
{
  DFANodeAlg newAlg(*this);
  newAlg.curpos++;
  return newAlg;
}

void DFANodeAlg::getAlgMeta(pair<int, int> &res) const
{
  res.first = algId;
  res.second = curpos;
}

bool DFANodeAlg::operator==(const DFANodeAlg &right) const
{
  return algId == right.algId && curpos == right.curpos && prospectCh == right.prospectCh;
}

// 产生式排序规则：产生式编号 > 当前位置 > 展望符
bool DFANodeAlg::operator<(const DFANodeAlg &right) const
{
  if (algId != right.algId)
  {
    return algId < right.algId;
  }
  if (curpos != right.curpos)
  {
    return curpos < right.curpos;
  }
  return prospectCh < right.prospectCh;
}

// -------- GrammarDFATransfer类的成员函数实现 --------
GrammarDFATransfer::GrammarDFATransfer(int _srcId, int _chId, int _dstId) : srcId(_srcId), chId(_chId), dstId(_dstId)
{
}
void GrammarDFATransfer::print()
{
  cout << srcId << " " << chId << " " << dstId << endl;
}

void GrammarDFATransfer::setDst(int _dstId)
{
  dstId = _dstId;
}

// -------- GrammarDFANode类的成员函数实现 --------
GrammarDFANode::GrammarDFANode(int _stateId) : stateId(_stateId)
{
}

void GrammarDFANode::addAlg(DFANodeAlg alg)
{
  nodeAlg.insert(alg);
  pair<int, int> algInfo;
  alg.getAlgMeta(algInfo);
  algIndex[algInfo] = alg;
}

// 状态结点排序规则
bool GrammarDFANode::operator<(const GrammarDFANode &right) const
{
  return nodeAlg < right.nodeAlg;
}

// 结点相同判定
bool GrammarDFANode::operator==(const GrammarDFANode &right) const
{
  return nodeAlg == right.nodeAlg;
}

// -------- GrammarDFA类的成员函数实现 --------
void GrammarDFA::expandNodeAlg(GrammarDFANode *node)
{
  for (set<DFANodeAlg>::iterator algItem = node->nodeAlg.begin(); algItem != node->nodeAlg.end(); algItem++)
  {
    pair<int, int> algInfo;
    algItem->getAlgMeta(algInfo);
    ProductionAlg &targetAlg = parser->algs[algInfo.first];
    if (algInfo.second == parser->algs[algInfo.first].rightAlg.size())
    {
      continue; // 该产生式已到结尾，不扩展
    }
    AlgElement *curElem = targetAlg.rightAlg[algInfo.second];
    if (curElem->getIsTerminal())
    {
      continue;
    }
    // 当前位置是非终结符：引入产生式
    set<int> curProspect;
    if (getProspectCh(*algItem, curProspect))
    {
      for (set<int>::iterator it = algItem->prospectCh.begin(); it != algItem->prospectCh.end(); it++)
      {
        curProspect.insert(*it);
      }
    }
    // 将所有该非终结符的产生式添加到newAlgs中
    for (int j = 0; j < parser->classifiedAlgs[curElem->getId()].size(); j++)
    {
      int curAlgId = parser->classifiedAlgs[curElem->getId()][j];
      if (node->algIndex.find({curAlgId, 0}) == node->algIndex.end())
      {
        // 未找到目标产生式，则添加该产生式
        node->addAlg(DFANodeAlg(curAlgId, 0, curProspect));
      }
      else
      {
        // 目标产生式已经在数组中，则更新产生式的展望符
        DFANodeAlg targetAlg(node->algIndex[{curAlgId, 0}]);
        node->nodeAlg.erase(targetAlg);
        for (set<int>::iterator it = curProspect.begin(); it != curProspect.end(); it++)
        {
          targetAlg.prospectCh.insert(*it);
        }
        node->addAlg(targetAlg);
      }
    }
  }
}

// 计算展望符，返回值为该产生式是否可以推导出空字
bool GrammarDFA::getProspectCh(const DFANodeAlg &alg, set<int> &res)
{
  pair<int, int> algInfo;
  alg.getAlgMeta(algInfo);
  ProductionAlg &targetAlg = parser->algs[algInfo.first];
  if (algInfo.second == targetAlg.rightAlg.size())
  {
    return true;
  }
  int i = algInfo.second + 1; // 从当前位置的下一个符号开始
  for (; i < targetAlg.rightAlg.size(); i++)
  {
    if (targetAlg.rightAlg[i]->getIsTerminal())
    {
      res.insert(targetAlg.rightAlg[i]->getId()); // 终结符：直接加入展望符中
      break;
    }
    // 非终结符：将该非终结符的FIRST集合作为展望符
    const set<int> elemFirst = parser->dict.getFirstSet(targetAlg.rightAlg[i]->getId());
    for (set<int>::iterator it = elemFirst.begin(); it != elemFirst.end(); it++)
    {
      if (*it != EMPTYWORDID)
      {
        res.insert(*it);
      }
    }
    if (elemFirst.find(EMPTYWORDID) == elemFirst.end())
    {
      break;
    }
  }
  return i == targetAlg.rightAlg.size();
}

void GrammarDFA::buildDFA()
{
  // 构建起始状态
  GrammarDFANode *startNode = new GrammarDFANode(0);
  startNode->addAlg(DFANodeAlg(0, 0, {ENDCHID}));
  queue<pair<GrammarDFANode *, GrammarDFATransfer *>> nodeQueue; // 使用队列记录未处理的结点，第二个元素记录的是与该状态有关的转移
  nodeQueue.push({startNode, nullptr});
  while (!nodeQueue.empty())
  {
    GrammarDFANode *curNode = nodeQueue.front().first;
    GrammarDFATransfer *nodeSrc = nodeQueue.front().second;
    nodeQueue.pop();
    expandNodeAlg(curNode); // 扩展等价产生式
    set<GrammarDFANode>::iterator findRes = dfa.find(*curNode);
    if (findRes != dfa.end())
    {
      // 消除重复状态：更改状态转移指向
      if (nodeSrc)
      {
        nodeSrc->setDst(findRes->stateId);
      }
      delete curNode;
      continue;
    }
    // 更改状态转移指向
    curNode->stateId = dfa.size();
    if (nodeSrc)
    {
      nodeSrc->setDst(curNode->stateId);
    }
    dfa.insert(*curNode);
    // 根据已有的产生式转移状态
    map<int, GrammarDFANode *> chTransfer; // 记录每个符号对应的转移状态
    for (set<DFANodeAlg>::iterator algItem = curNode->nodeAlg.begin(); algItem != curNode->nodeAlg.end(); algItem++)
    {
      pair<int, int> algInfo;
      algItem->getAlgMeta(algInfo);
      algItem->getAlgMeta(algInfo);
      // !!! debug
      cout << curNode->stateId << " " << algInfo.first << " [" << algInfo.second << "] (";
      for (set<int>::iterator it = algItem->prospectCh.begin(); it != algItem->prospectCh.end(); it++)
      {
        cout << *it << " ";
      }
      cout << ")" << endl;
      // ---- !!! debug
      if (algInfo.second == parser->algs[algInfo.first].rightAlg.size())
      {
        continue; // 该产生式已到结尾，不需要转移
      }
      int curCh = parser->algs[algInfo.first].rightAlg[algInfo.second]->getId(); // 该产生式接受的字符
      if (chTransfer.find(curCh) == chTransfer.end())
      {
        // 没有接受该字符的新状态，创建状态
        GrammarDFANode *newState = new GrammarDFANode;
        chTransfer[curCh] = newState;
        GrammarDFATransfer *newTransfer = new GrammarDFATransfer(curNode->stateId, curCh);
        transfer.push_back(newTransfer);
        nodeQueue.push({newState, newTransfer});
      }
      chTransfer[curCh]->addAlg(algItem->createNext());
    }
    delete curNode;
  }
  // !!! debug
  for (int i = 0; i < transfer.size(); i++)
  {
    transfer[i]->print();
  }
  // ---- !!! debug
}

void GrammarDFA::linkParser(GrammarParser *_parser)
{
  parser = _parser;
}

GrammarDFA::~GrammarDFA()
{
  for (int i = 0; i < transfer.size(); i++)
  {
    delete transfer[i];
  }
}

// -------- GrammarParser类的成员函数实现 --------
void GrammarParser::processGrammarRule()
{
  try
  {
    int num;
    cin >> num;
    cin.ignore(100000000, '\n');
    dict.init(this);
    for (int i = 0; i < num; i++)
    {
      string s;
      getline(cin, s);
      ProductionAlg alg;
      alg.readFromStr(s, dict);
      algs.push_back(alg);
    }
    ProductionAlg::splitAlg(algs);
    // !!! debug
    for (int i = 0; i < algs.size(); i++)
    {
      cout << algs[i].id << ": " << algs[i].leftElem->getId() << " -> ";
      for (int j = 0; j < algs[i].rightAlg.size(); j++)
      {
        cout << algs[i].rightAlg[j]->getId() << " ";
      }
      cout << endl;
    }
    // --- !!! debug
    dict.classifyAlg();
    dict.calculateFirst();
    GrammarDFA gdfa;
    gdfa.linkParser(this);
    gdfa.buildDFA();
  }
  catch (InputException &e)
  {
    cerr << e.what() << endl;
  }
}

void GrammarParser::LR1Main()
{
}

// !!! debug
int main()
{
  GrammarParser parser;
  parser.processGrammarRule();
  cout << "FINISH" << endl;
  return 0;
}
