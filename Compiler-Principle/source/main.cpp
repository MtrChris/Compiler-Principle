#include "Grammar/GrammarParser.h"

int main()
{
  GrammarParser parser;
  parser.processGrammarRule();
  parser.LR1Main();
  return 0;
}