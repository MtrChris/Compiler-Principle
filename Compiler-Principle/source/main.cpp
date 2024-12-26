#include "Grammar/GrammarParser.h"
#include "Lex/Lex.h"

int main()
{
  try{
    GrammarParser parser;
    parser.processGrammarRule();
    parser.LR1Main();
    std::cout << "生成成功" << std::endl;
  }
  catch(GrammarException& e){
    std::cout << "由于发生语法错误，生成终止：" << std::endl;
    std::cout << e.what() << std::endl;
  }
  catch(LexException& e){
    std::cout << "由于发生词法错误，生成终止：" << std::endl;
    std::cout << e.what() << std::endl;
  }
  return 0;
}