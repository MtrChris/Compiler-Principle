#include "GrammarParser.h"
#include "InputHandler.h"
#include "CodeGenerator.h"
#include "../Lex/Lex.h"
#include <queue>
#include <stack>
#include <fstream>
#include <iomanip>

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

void AlgElement::print(ofstream& outFile) const
{
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

int ElementDict::findElem(const AlgElement &elem) const
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

int ElementDict::findElem(const string &elemName) const
{
  return findElem(NonTerminalElement(elemName));
}

int ElementDict::findElem(const string &elemType, const string &elemVal) const
{
  return findElem(TerminalElement(elemType, elemVal));
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

AlgElement *ElementDict::getElem(int index) const
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
        if (*it != EMPTYWORDID)
        {
          firstSet[curElem].insert(*it);
        }
      }
      // 该非终结符不包含空字时，查找下一个产生式
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

const set<int> &ElementDict::getFirstSet(int id) const
{
  return firstSet[id];
}

int ElementDict::getDictLength() const
{
  return elemList.size();
}

void ElementDict::print(std::ofstream& outFile) const
{
  for (int i = 0; i < elemList.size(); i++)
  {
    elemList[i]->print(outFile);
  }
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

const string& TerminalElement::getType() const
{
  return type;
}

const string& TerminalElement::getVal() const
{
  return val;
}

bool TerminalElement::operator==(const AlgElement &right)
{
  const TerminalElement *tRight = dynamic_cast<const TerminalElement *>(&right);
  if (!tRight)
  {
    return false;
  }
  return type == tRight->type;
}

void TerminalElement::print(std::ofstream& outFile) const
{
  outFile << "id: " << getId() << " type: " << type << " " << " val: " << val << std::endl;
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

void NonTerminalElement::print(std::ofstream& outFile) const
{
  outFile << "id: " << getId() << " name: " << name << std::endl;
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

AlgElement *ProductionAlg::getLeftElem() const
{
  return leftElem;
}

AlgElement *ProductionAlg::getAlgElement(int index) const
{
  return rightAlg[index];
}

int ProductionAlg::getAlgLength() const
{
  return rightAlg.size();
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

int DFANodeAlg::getAlgId() const
{
  return algId;
}

int DFANodeAlg::getAlgPos() const
{
  return curpos;
}

const ProductionAlg &DFANodeAlg::getFullAlg(GrammarParser *parser) const
{
  return parser->getProductionAlg(algId);
}

AlgElement *DFANodeAlg::getCurElement(GrammarParser *parser) const
{
  return parser->getProductionAlg(algId).getAlgElement(curpos);
}

bool DFANodeAlg::isAtEnd(GrammarParser *parser) const
{
  return parser->getProductionAlg(algId).getAlgLength() == curpos;
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

int GrammarDFATransfer::getSrc()
{
  return srcId;
}

int GrammarDFATransfer::getDstId()
{
  return dstId;
}

int GrammarDFATransfer::getChId()
{
  return chId;
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

const set<DFANodeAlg> &GrammarDFANode::getAlgs() const
{
  return nodeAlg;
}

int GrammarDFANode::getId() const
{
  return stateId;
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
  queue<const DFANodeAlg *> q; // 记录未进行扩展的表达式编号
  for (set<DFANodeAlg>::iterator it = node->nodeAlg.begin(); it != node->nodeAlg.end(); it++)
  {
    q.push(&(*it));
  }
  while (!q.empty())
  {
    const DFANodeAlg *algItem = q.front();
    q.pop();
    if (algItem->isAtEnd(parser))
    {
      continue; // 该产生式已到结尾，不扩展
    }
    AlgElement *curElem = algItem->getCurElement(parser);
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
    // 将所有该非终结符的产生式添加到节点中
    const vector<int> &chAlgs = parser->getClassifiedAlgs(curElem->getId());
    for (int j = 0; j < chAlgs.size(); j++)
    {
      bool expandFlag = true;
      if (node->algIndex.find({chAlgs[j], 0}) == node->algIndex.end())
      {
        // 未找到目标产生式，则添加该产生式
        node->addAlg(DFANodeAlg(chAlgs[j], 0, curProspect));
      }
      else
      {
        // 目标产生式已经在数组中，则更新产生式的展望符
        expandFlag = false; // 暂定产生式不需要再次扩展
        DFANodeAlg targetAlg(node->algIndex[{chAlgs[j], 0}]);
        node->nodeAlg.erase(targetAlg);
        for (set<int>::iterator it = curProspect.begin(); it != curProspect.end(); it++)
        {
          if (targetAlg.prospectCh.find(*it) == targetAlg.prospectCh.end())
          {
            targetAlg.prospectCh.insert(*it);
            expandFlag = true; // 产生式更新了展望符，需要重新扩展
          }
        }
        node->addAlg(targetAlg);
      }
      if (expandFlag)
      {
        q.push(&node->algIndex[{chAlgs[j], 0}]);
      }
    }
  }
}

// 计算展望符，返回值为该产生式是否可以推导出空字
bool GrammarDFA::getProspectCh(const DFANodeAlg &alg, set<int> &res)
{
  const ProductionAlg &targetAlg = alg.getFullAlg(parser);
  if (alg.isAtEnd(parser))
  {
    return true;
  }
  int i = alg.getAlgPos() + 1; // 从当前位置的下一个符号开始
  for (; i < targetAlg.getAlgLength(); i++)
  {
    if (targetAlg.getAlgElement(i)->getIsTerminal())
    {
      res.insert(targetAlg.getAlgElement(i)->getId()); // 终结符：直接加入展望符中
      break;
    }
    // 非终结符：将该非终结符的FIRST集合作为展望符
    const set<int> elemFirst = parser->getDict().getFirstSet(targetAlg.getAlgElement(i)->getId());
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
      if (algItem->isAtEnd(parser))
      {
        continue; // 该产生式已到结尾，不需要转移
      }
      int curCh = algItem->getCurElement(parser)->getId(); // 该产生式接受的字符
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
}

const set<GrammarDFANode> &GrammarDFA::getNodes() const
{
  return dfa;
}

void GrammarDFA::linkParser(GrammarParser *_parser)
{
  parser = _parser;
}

const vector<GrammarDFATransfer *> &GrammarDFA::getTransfer() const
{
  return transfer;
}

GrammarDFA::~GrammarDFA()
{
  for (int i = 0; i < transfer.size(); i++)
  {
    delete transfer[i];
  }
}

// -------- LRItem类的成员函数实现 -------
LRItem::LRItem(ActionType _action, int _index) : action(_action), index(_index)
{
}

void LRItem::setItem(ActionType _action, int _index)
{
  action = _action;
  index = _index;
}

// -------- LRChart类的成员函数实现 --------
void LRChart::init(GrammarParser *_parser)
{
  parser = _parser;
  chart.resize(_parser->getDFA().getNodes().size(), vector<LRItem>(_parser->getDict().getDictLength()));
}

void LRChart::build()
{
  // 1. 将每个转移对应到SHIFT和GOTO
  const vector<GrammarDFATransfer *> &dfaTransfer = parser->getDFA().getTransfer();
  for (vector<GrammarDFATransfer *>::const_iterator tsf = dfaTransfer.begin(); tsf < dfaTransfer.end(); tsf++)
  {
    ActionType tsfAction = (parser->getDict().getElem((*tsf)->getChId())->getIsTerminal()) ? SHIFT : GOTO;
    chart[(*tsf)->getSrc()][(*tsf)->getChId()].setItem(tsfAction, (*tsf)->getDstId());
  }
  // 2. 遍历每个状态，对应到REDUCE
  const set<GrammarDFANode> &dfaNodes = parser->getDFA().getNodes();
  for (set<GrammarDFANode>::const_iterator node = dfaNodes.begin(); node != dfaNodes.end(); node++)
  {
    const set<DFANodeAlg> &nodeAlgs = node->getAlgs();
    for (set<DFANodeAlg>::const_iterator alg = nodeAlgs.begin(); alg != nodeAlgs.end(); alg++)
    {
      if (alg->isAtEnd(parser)) // 对于已经到结尾的产生式，置为REDUCE
      {
        for (set<int>::iterator ch = alg->prospectCh.begin(); ch != alg->prospectCh.end(); ch++)
        {
          chart[node->getId()][*ch].setItem(REDUCE, alg->getAlgId());
        }
      }
    }
  }
  // 3. 设置ACCEPT状态：由于添加了起始产生式，ACCEPT状态必定处于1状态
  chart[ACCSTATEID][ENDCHID].setItem(ACCEPT);
}

const LRItem &LRChart::get(int state, int ch) const
{
  return chart[state][ch];
}

void LRChart::print(ofstream& outFile) const
{
  outFile << setiosflags(ios::left);
  outFile << setw(4) << " ";
  for (int ch = 0; ch < parser->getDict().getDictLength(); ch++)
  {
    outFile << setw(4) << ch;
  }
  outFile << endl;
  for (int state = 0; state < parser->getDFA().getNodes().size(); state++)
  {
    outFile << setw(4) << state;
    for (int ch = 0; ch < parser->getDict().getDictLength(); ch++)
    {
      const LRItem &item = chart[state][ch];
      switch (item.action)
      {
      case ACCEPT:
        outFile << "ACC ";
        break;
      case ERROR:
        outFile << "ERR ";
        break;
      case SHIFT:
        outFile << "S" << setw(3) << item.index;
        break;
      case REDUCE:
        outFile << "R" << setw(3) << item.index;
        break;
      case GOTO:
        outFile << "G" << setw(3) << item.index;
        break;
      }
    }
    outFile << endl;
  }
}

// -------- GrammarParser类的成员函数实现 --------
const ProductionAlg &GrammarParser::getProductionAlg(int index)
{
  return algs[index];
}

const GrammarDFA &GrammarParser::getDFA() const
{
  return dfa;
}

const std::vector<int> &GrammarParser::getClassifiedAlgs(int ch)
{
  return classifiedAlgs[ch];
}

const ElementDict &GrammarParser::getDict() const
{
  return dict;
}

void GrammarParser::printAlgs(ofstream& outFile) const
{
  for (int i = 0; i < algs.size(); i++)
  {
    outFile << algs[i].id << ": " << algs[i].leftElem->getId() << " -> ";
    for (int j = 0; j < algs[i].rightAlg.size(); j++)
    {
      outFile << algs[i].rightAlg[j]->getId() << " ";
    }
    outFile << endl;
  }
}

void GrammarParser::processGrammarRule()
{
  try
  {
    ifstream grammarInput;
    grammarInput.open("grammarInput.txt");
    if (!grammarInput.is_open())
    {
      throw InputException("无法打开grammarInput.txt进行读入");
    }
    dict.init(this);
    string s;
    while (getline(grammarInput, s))
    {
      ProductionAlg alg;
      alg.readFromStr(s, dict);
      algs.push_back(alg);
    }
    ProductionAlg::splitAlg(algs);
    dict.classifyAlg();
    dict.calculateFirst();
    dfa.linkParser(this);
    dfa.buildDFA();
    chart.init(this);
    chart.build();
    printLog();
    // 以下为打印操作
  }
  catch (InputException &e)
  {
    cerr << e.what() << endl;
  }
}

void GrammarParser::LR1Main()
{
  prepareLex();
  stack<int> stateStack;
  stack<Expr *> symbolStack;
  stateStack.push(0);
  CodeGenerator codegenerator;
  int curTerminal = -1;
  int curSymbol = -1;
  while (true)
  {
    if(curSymbol == -1){
      NametabItem curItem;
      int res = readNext(curItem);
      if(res == FINISHED){
        curTerminal = ENDCHID;
      }
      else{
        curTerminal = dict.findElem(curItem.type, curItem.name);
        cout << curItem.type << " " << curItem.name << endl; // !!! debug
      }
      curSymbol = curTerminal;
    }
    int currentState = stateStack.top();
    AlgElement* curElement = dict.getElem(curSymbol);
    LRItem item = chart.get(currentState, curSymbol);

    switch (item.action)
    {
    case SHIFT:
    {
      stateStack.push(item.index);
      Expr *shiftE = new Expr();
      TerminalElement* terminalPtr = dynamic_cast<TerminalElement*>(curElement);
      if(terminalPtr && terminalPtr->getType() == SYMBOL){
        shiftE->place = terminalPtr->getVal();
      }
      symbolStack.push(shiftE);
      curSymbol = -1;
      break;
    }
    case REDUCE:
    {
      ProductionAlg prod = getProductionAlg(item.index);
      for(int i = 0; i < prod.getAlgLength(); i++){
        stateStack.pop();
      }
      // codegenerator.GenerateCode(prod, stateStack, symbolStack);
      curSymbol = prod.leftElem->getId();
      break;
    }

    case GOTO:
      stateStack.push(item.index);
      curSymbol = curTerminal;
      break;

    case ACCEPT:
      return;

    case ERROR:
      throw GrammarException("语法分析错误：非法语句构成");
      return;
    default:
      throw GrammarException("语法分析错误：LR分析表元素非法");
      return;
    }
  }
}

void GrammarParser::printLog() const
{
  ofstream& outFile = LogHandler::getInstance();
  cout << "---------- 产生式表 ----------" << endl;
  printAlgs(outFile);
  cout << endl;
  cout << "---------- 符号表 -------- " << endl;
  dict.print(outFile);
  cout << endl;
  cout << "---------- LR分析表 -----------" << endl;
  chart.print(outFile);
  cout << endl;
}

// -------- GrammarException类的成员函数实现 --------
GrammarException::GrammarException(const std::string &msg)
{
  _msg = msg;
}

const char *GrammarException::what()
{
  return _msg.c_str();
}
