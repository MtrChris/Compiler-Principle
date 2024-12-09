#include <vector>
#include <string>
#include <set>
#include <map>
#include <iostream> // !!! debug

#define EMPTYWORDID 1
#define ENDCHID 2

class ProductionAlg;
class GrammarParser;

// 产生式元素
class AlgElement
{
  friend class ElementDict;
  int id;

protected:
  bool isTerminal; // 是否为终结符
public:
  AlgElement();
  bool getIsTerminal() const;
  int getId() const;
  virtual bool operator==(const AlgElement &right);
  virtual void print() {} // !!! debug
};

// 终结符类
class TerminalElement : public AlgElement
{
  std::string type;
  std::string val;

public:
  TerminalElement(std::string _type = "", std::string _val = "");
  bool operator==(const AlgElement &right) override;
  void print()
  {
    std::cout << "id: " << getId() << " type: " << type << " " << " val: " << val << std::endl;
  }
};

// 非终结符类
class NonTerminalElement : public AlgElement
{
  std::string name; // 非终结符符号串
public:
  NonTerminalElement(std::string _name = "");
  bool operator==(const AlgElement &right) override;
  void print()
  {
    std::cout << "id: " << getId() << " name: " << name << std::endl;
  }
};

// 元素表类的定义，存储产生式中的所有元素的信息
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
  static TerminalElement *const CANDIDATEMARK; // 候选式符 |
  static TerminalElement *const EMPTYWORD;     // 空字
  static TerminalElement *const ENDCH;         // 结束符
  static NonTerminalElement *const STARTSYMBOL;
  void init(GrammarParser *_parser);
  void createElem(AlgElement *elem);
  int findElem(const AlgElement &elem);
  int findElem(const std::string &elemName);
  AlgElement *getElem(int index);
  void updateElem(AlgElement *&elem);
  void classifyAlg();
  void calculateFirst();
  const std::set<int> &getFirstSet(int id);
  ~ElementDict();
};

// 产生式类
class ProductionAlg
{
  // 产生式：leftElem -> rightAlg
public:
  int id;
  AlgElement *leftElem;
  std::vector<AlgElement *> rightAlg;

public:
  ProductionAlg(AlgElement *_leftElem = nullptr);
  ProductionAlg(AlgElement *_leftElem, std::vector<AlgElement *> &_rightAlg);
  void readFromStr(const std::string &s, ElementDict &dict);
  static void splitAlg(std::vector<ProductionAlg> &algs);
};

// DFA结点的产生式类
class DFANodeAlg
{
  int algId;
  int curpos; // 当前阅读的位置
public:
  std::set<int> prospectCh; // 展望符
  DFANodeAlg(int _algId = -1, int _curpos = -1, std::set<int> prospectCh = std::set<int>());
  DFANodeAlg createNext() const;
  void getAlgMeta(std::pair<int, int> &res) const;
  bool operator==(const DFANodeAlg &right) const;
  bool operator<(const DFANodeAlg &right) const;
};

// 语法DFA转移类
class GrammarDFATransfer
{
  int srcId;
  int chId;
  int dstId;

public:
  GrammarDFATransfer(int _srcId, int _chId = -1, int _dstId = -1);
  void setDst(int _dstId);
  // !!! debug
  void print();
};

// 语法DFA结点类
class GrammarDFANode
{
  friend class GrammarDFA;
  int stateId;
  std::set<DFANodeAlg> nodeAlg;
  std::map<std::pair<int, int>, DFANodeAlg> algIndex;

public:
  GrammarDFANode(int _stateId = -1);
  void addAlg(DFANodeAlg alg);
  bool operator<(const GrammarDFANode &right) const;
  bool operator==(const GrammarDFANode &right) const;
};

// 语法DFA类
class GrammarDFA
{
  GrammarParser *parser;
  std::set<GrammarDFANode> dfa;
  std::vector<GrammarDFATransfer *> transfer;
  void expandNodeAlg(GrammarDFANode *node); // 对DFA结点的产生式进行扩展
  bool getProspectCh(const DFANodeAlg &alg, std::set<int> &res);

public:
  ~GrammarDFA();
  void linkParser(GrammarParser *_parser);
  void buildDFA();
};

// 语法分析器类
class GrammarParser
{
  friend class GrammarDFA;
  friend class ElementDict;
  std::vector<ProductionAlg> algs;
  std::vector<std::vector<int>> classifiedAlgs;
  ElementDict dict;

public:
  void processGrammarRule();
  void LR1Main();
};