#include <vector>
#include <string>
#include <iostream> // !!! debug

#define CANDIDATEMARK 0

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
  std::vector<AlgElement *> elemList;

public:
  void init();
  void createElem(AlgElement *elem); // 创建一个元素
  int findElem(const AlgElement &elem);
  int findElem(const std::string &elemName);
  AlgElement *getElem(int index);
  void updateElem(AlgElement *&elem);
  ~ElementDict();
};

// 语法分析器类的定义
class GrammarParser
{
public:
  void readGrammarRule();
  void LR1Main();
};

// 产生式类
class ProductionAlg
{
  // 产生式：leftElem -> rightAlg
public:
  AlgElement *leftElem;
  std::vector<AlgElement *> rightAlg;
  char prospectCh; // 展望符
public:
  ProductionAlg();
  void readFromStr(const std::string &s, ElementDict &dict);
};
