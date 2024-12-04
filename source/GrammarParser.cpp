#include "GrammarParser.h"
#include "InputHandler.h"
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
void ElementDict::init()
{
  elemList.clear();
  createElem(new TerminalElement("CANDIDATEMARK"));
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
    delete elem;
    elem = elemList[findRes];
  }
}

AlgElement *ElementDict::getElem(int index)
{
  return elemList[index];
}

ElementDict::~ElementDict()
{
  for (int i = 0; i < elemList.size(); i++)
  {
    delete elemList[i];
  }
}

// -------- GrammarParser类的成员函数实现 --------
void GrammarParser::readGrammarRule()
{
}

void GrammarParser::LR1Main()
{
}

// -------- TerminalElement类的成员函数实现  --------

TerminalElement::TerminalElement(string _type, string _val) : type(_type), val(_val)
{
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
ProductionAlg::ProductionAlg()
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
      elem = new TerminalElement("CANDIDATEMARK");
    }
    else
    {
      elem = new NonTerminalElement(word);
    }
    dict.updateElem(elem);
    rightAlg.push_back(elem);
  }
}

// !!! debug
/*
int main()
{
  try
  {
    string s;
    getline(cin, s);
    ProductionAlg alg;
    ElementDict dict;
    dict.init();
    alg.readFromStr(s, dict);
    alg.leftElem->print();
    for (int i = 0; i < alg.rightAlg.size(); i++)
    {
      alg.rightAlg[i]->print();
    }
  }
  catch (InputException &e)
  {
    cerr << e.what() << endl;
  }
  return 0;
}
*/