#include "InputHandler.h"
using namespace std;

// -------- InputReader类的函数实现 --------
InputReader::InputReader(const string &input)
{
  init(input);
}

void InputReader::skipWhiteSpace()
{
  for (; curpos < text.end() && *curpos == ' '; curpos++)
  {
    ;
  }
}

void InputReader::init(const string &input)
{
  text = input;
  curpos = text.begin();
  if (text.size() > 0 && text[text.size() - 1] == '\n')
  {
    text.pop_back();
  }
}

bool InputReader::hasNextWord()
{
  skipWhiteSpace();
  return curpos < text.end();
}

void InputReader::getNextWord(std::string &word)
{
  if (!hasNextWord())
  {
    throw InputException("没有下一个可读取的字符");
  }
  word.clear();
  for (; curpos < text.end() && *curpos != ' '; curpos++)
  {
    word.push_back(*curpos);
  }
}

char InputReader::getNextCh()
{
  if (!hasNextWord())
  {
    throw InputException("没有下一个可读取的字符");
  }
  return *(curpos++);
}

void InputReader::removeEscape(string &s)
{
  string res;
  for (string::iterator it = s.begin(); it < s.end(); it++)
  {
    if (*it != '\\')
    {
      res.push_back(*it);
    }
  }
  s = res;
}

// -------- InputException类的函数实现 --------
InputException::InputException(const std::string &msg)
{
  _msg = msg;
}
const char *InputException::what()
{
  return _msg.c_str();
}
