#include <string>
#include <exception>

class InputException : public std::exception
{
  std::string _msg;

public:
  InputException(const std::string &msg = "输入异常");
  const char *what();
};

// 输入读取类
class InputReader
{
  std::string text;
  std::string::const_iterator curpos;
  void skipWhiteSpace();

public:
  InputReader(const std::string &input = "");
  void init(const std::string &input);
  bool hasNextWord();
  void getNextWord(std::string &word);
  char getNextCh();
  static void removeEscape(std::string &s);
};