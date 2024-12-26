#pragma once
#include <vector>
#include <string>
#include <set>
#include <map>
#include <exception>
#include <iostream> // !!! debug
#include <iomanip>  // !!! debug

#define EMPTYWORDID 1
#define ENDCHID 2
#define ACCSTATEID 1

class ProductionAlg;
class GrammarParser;

// ����ʽԪ��
class AlgElement
{
  friend class ElementDict;
  int id;

protected:
  bool isTerminal; // �Ƿ�Ϊ�ս��
public:
  AlgElement();
  inline bool getIsTerminal() const;
  inline int getId() const;
  virtual bool operator==(const AlgElement &right);
  virtual void print() {} // !!! debug
};

// �ս����
class TerminalElement : public AlgElement
{
  std::string type;
  std::string val;

public:
  TerminalElement(std::string _type = "", std::string _val = "");
  bool operator==(const AlgElement &right) override;
  inline const std::string& getType() const;
  inline const std::string& getVal() const;
  void print()
  {
    std::cout << "id: " << getId() << " type: " << type << " " << " val: " << val << std::endl;
  }
};

// ���ս����
class NonTerminalElement : public AlgElement
{
  std::string name; // ���ս�����Ŵ�
public:
  NonTerminalElement(std::string _name = "");
  bool operator==(const AlgElement &right) override;
  void print()
  {
    std::cout << "id: " << getId() << " name: " << name << std::endl;
  }
};

// Ԫ�ر���Ķ��壬�洢����ʽ�е�����Ԫ�ص���Ϣ
class ElementDict
{
private:
  enum SetState
  {
    UNCALCULATED,
    CALCULATING,
    FINISHED,
  };

  std::vector<AlgElement *> elemList;
  std::vector<std::set<int>> firstSet;
  GrammarParser *parser;
  void _calculateFirst(int curElem, std::vector<bool> &calcState);

public:
  static TerminalElement *const CANDIDATEMARK; // ��ѡʽ�� |
  static TerminalElement *const EMPTYWORD;     // ����
  static TerminalElement *const ENDCH;         // ������
  static NonTerminalElement *const STARTSYMBOL;
  void init(GrammarParser *_parser);
  void createElem(AlgElement *elem);
  int findElem(const AlgElement &elem) const;
  int findElem(const std::string &elemName) const;
  int findElem(const std::string &elemType, const std::string &elemVal) const;
  inline AlgElement *getElem(int index) const;
  void updateElem(AlgElement *&elem);
  void classifyAlg();
  void calculateFirst();
  inline const std::set<int> &getFirstSet(int id) const;
  inline int getDictLength() const;
  void print() const;
  ~ElementDict();
};

// ����ʽ��
class ProductionAlg
{
  // ����ʽ��leftElem -> rightAlg
public:
  int id;
  AlgElement *leftElem;
  std::vector<AlgElement *> rightAlg;

public:
  ProductionAlg(AlgElement *_leftElem = nullptr);
  ProductionAlg(AlgElement *_leftElem, std::vector<AlgElement *> &_rightAlg);
  void readFromStr(const std::string &s, ElementDict &dict);
  static void splitAlg(std::vector<ProductionAlg> &algs);
  inline AlgElement *getAlgElement(int index) const;
  inline int getAlgLength() const;
  inline AlgElement *getLeftElem() const;
};

// DFA���Ĳ���ʽ��
class DFANodeAlg
{
  int algId;
  int curpos; // ��ǰ�Ķ���λ��
public:
  std::set<int> prospectCh; // չ����
  DFANodeAlg(int _algId = -1, int _curpos = -1, std::set<int> prospectCh = std::set<int>());
  DFANodeAlg createNext() const;
  void getAlgMeta(std::pair<int, int> &res) const;
  inline int getAlgId() const;
  inline int getAlgPos() const;
  const ProductionAlg &getFullAlg(GrammarParser *parser) const;
  AlgElement *getCurElement(GrammarParser *parser) const;
  bool isAtEnd(GrammarParser *parser) const;
  bool operator==(const DFANodeAlg &right) const;
  bool operator<(const DFANodeAlg &right) const;
};

// �﷨DFAת����
class GrammarDFATransfer
{
  int srcId;
  int chId;
  int dstId;

public:
  GrammarDFATransfer(int _srcId, int _chId = -1, int _dstId = -1);
  void setDst(int _dstId);
  inline int getSrc();
  inline int getChId();
  inline int getDstId();
  // !!! debug
  void print();
};

// �﷨DFA�����
class GrammarDFANode
{
  friend class GrammarDFA;
  int stateId;
  std::set<DFANodeAlg> nodeAlg;
  std::map<std::pair<int, int>, DFANodeAlg> algIndex;

public:
  GrammarDFANode(int _stateId = -1);
  void addAlg(DFANodeAlg alg);
  inline int getId() const;
  inline const std::set<DFANodeAlg> &getAlgs() const;
  bool operator<(const GrammarDFANode &right) const;
  bool operator==(const GrammarDFANode &right) const;
};

// �﷨DFA��
class GrammarDFA
{
  GrammarParser *parser;
  std::set<GrammarDFANode> dfa;
  std::vector<GrammarDFATransfer *> transfer;
  void expandNodeAlg(GrammarDFANode *node); // ��DFA���Ĳ���ʽ������չ
  bool getProspectCh(const DFANodeAlg &alg, std::set<int> &res);

public:
  ~GrammarDFA();
  void linkParser(GrammarParser *_parser);
  void buildDFA();
  inline const std::vector<GrammarDFATransfer *> &getTransfer() const;
  inline const std::set<GrammarDFANode> &getNodes() const;
};

// LR��������Ŀ��
enum ActionType
{
  ACCEPT,
  SHIFT,
  REDUCE,
  GOTO,
  ERROR
};

class LRItem
{
public:
  ActionType action;
  int index;
  LRItem(ActionType _action = ERROR, int _index = -1);
  void setItem(ActionType _action, int _index = -1);
};

// LR��������
class LRChart
{
  std::vector<std::vector<LRItem>> chart;
  GrammarParser *parser;

public:
  void init(GrammarParser *_parser);
  void build();
  inline const LRItem &get(int state, int ch) const;
  void print() const;
};

// �﷨��������
class GrammarParser
{
  friend class ElementDict;
  std::vector<ProductionAlg> algs;
  std::vector<std::vector<int>> classifiedAlgs;
  ElementDict dict;
  LRChart chart;
  GrammarDFA dfa;

public:
  inline const ProductionAlg &getProductionAlg(int index);
  inline const std::vector<int> &getClassifiedAlgs(int ch);
  inline const GrammarDFA &getDFA() const;
  inline const ElementDict &getDict() const;
  void printAlgs() const;
  void processGrammarRule();
  void LR1Main();
};

class GrammarException : public std::exception
{
  std::string _msg;

public:
  GrammarException(const std::string &msg = "�﷨��������");
  const char *what();
};