#pragma once

class ElementAttr
{
public:
  int place; // 表达式临时变量编号
  int truelist = -1;
  int falselist = -1;
  int nextlist = -1;
  int quad = -1;
};
