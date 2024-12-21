# Compiler-Principle

# 词法分析完成情况

完成从正规式到NFA的转化

完成NFA到DFA的转化

完成DFA的化简

## 使用说明

在随意txt中写入文法，目前仅支持ASCII码。

格式如下：

```
_
345>a|b|c
123>aabb{345}
```

第一行输入除a-z,A-Z,0-9外还支持识别的字符，字符之间不要有空格，除非你想要识别空格，换行符不支持。

后面的几行输入正规式：

标识符>正规式

不要有任何多余的字符。

正规式中仅允许出现“*”，“|”，“(”,")"算符和定义的可识别符号。

标识符中不能有“>”,“{”,“}”字符。

支持在正规式中插入标识符，需要用{}括起来。

但是一旦某标识符被插入正规式，它将不再参与最后的自动机的生成，仅作为自动机生成的中间产物。

如：

```
letter>a|b|c|d|e|f
word>{letter}*
```

最后的自动机显然不会输出结果letter。

同时，使用的标识符必须在前面已经定义过。

## 测试说明：

目前完成了NFA到DFA的转换，DFA的化简，“分叉情况”已经解决

测试用例1：

```
345>a|b|c
123>aabb{345}
```

化简前结果：（出现分叉）

```
0    1   a
1    2   a
2    3   b
3    4   b
4    5   a  终态:123
4    6   b  终态:123
4    7   c  终态:123
```

化简后结果：（没有分叉）

```
0    0   a
0    1   b
1    2   b
2    3   a b c   Final State:123
```

测试用例2：

```
123>(a|b|c|d)*abc
```

化简前结果：

```
0    1   a
0    2   b
0    3   c
0    4   d
1    1   a
1    3   c
1    4   d
1    5   b
2    1   a
2    2   b
2    3   c
2    4   d
3    1   a
3    2   b
3    3   c
3    4   d
4    1   a
4    2   b
4    3   c
4    4   d
5    1   a
5    2   b
5    4   d
5    6   c  终态:123
6    1   a
6    2   b
6    3   c
6    4   d
```

化简后结果：

```
0    0   b c d
0    1   a
1    0   c d
1    1   a
1    2   b
2    0   b d
2    1   a
2    3   c   Final State:123
3    0   b c d
3    1   a
```

## 代码说明：

自动机的定义：` Automaton`类 在`Lex.h`中

类成员：

` begin`:自动机的起始状态编号（可不要）

`end`:自动机的结束状态编号（可不要）

`map<uint64,bitset<MAXCH>>edges`:存储边的集合

键为起点和终点的复合，64位`unsigned long long`。高32位为起点，低32位为终点。值为位图，存储发生状态转移的字符。

辅助宏：

```cpp
using uint64 =unsigned long long int;
//取起点
#define BEGIN(x) ((int)((uint64)(x)>>32)) 
//起点和终点合成边
#define EDGE(x,y) (((uint64)(x)<<32)+(uint64)(y))
//取终点
#define END(x) ((int)(x))
//将边x的起点和终点同时偏移y
#define OFFSET(x,y) ((uint64)(x)+((uint64)(y)<<32)+y)
```

工具函数：

```cpp
struct edges {
	std::map<uint64, std::bitset<MAXCH>>::iterator begin;
	std::map<uint64, std::bitset<MAXCH>>::iterator end;
}
//从边集中获取以state为起点的边的范围[,)
//LexTool.cpp
edges getEdges(int state,map<uint64,bitset<MAXCH>>& edge)
{
    struct edges p;
    p.begin=edge.lower_bound((uint64)state << 32);
    p.end = edge.lower_bound((uint64)(state + 1) << 32);
    return p;
}
```

主控函数位于LexMaster.cpp

指挥整个词法分析器的运行。

<h1>LR分析表完成版本说明（编辑于12-09）</h1>
<h2>测试说明</h2>

该版本已经完成了LR分析表的构建，可以进行测试

输入的第一行为产生式的个数，然后从第二行开始每行输入一个产生式，输入格式请参照 <strong>语法输入规则</strong> 部分的说明

可以使用不同用例进行测试，程序已经可以根据课程ppt中的例子生成正确的LR分析表

测试用例：

Test 1:

```
3
S -> B B
B -> { letter b } B
B -> { letter a }
```

Test 2:

```
4
value_expression -> sub_expression | sub_expression { operator > } sub_expression | sub_expression { operator < } sub_expression | sub_expression { operator == } sub_expression | sub_expression { operator >= } sub_expression | sub_expression { operator <= } sub_expression | sub_expression { operator != } sub_expression
sub_expression -> subsub_expression | subsub_expression { operator + } sub_expression | subsub_expression { operator - } sub_expression
subsub_expression -> value | value { operator * } subsub_expression | value { operator / } subsub_expression
value -> { NUM }
```

Test 3

```
5
E  -> T E'
E' -> { operator + } T E' | { }
T  -> F T'
T' -> { operator * } F T' | { }
F -> { bracket ( } E { bracket ) } | { symbol i }
```

<h2>代码对接说明</h2>

LR分析表存储在`LRChart`类中，分析表中的每个项目为`LRItem`类。`LRItem`的`action`字段为操作类型，该枚举常量类型包括`SHIFT`、`REDUCE`、`GOTO`、`ACCEPT`、`ERROR`，另一个字段`index`表示操作数，即转移到哪个状态或使用哪个产生式进行归约

可以使用的函数：

`GrammarParser`类：

- `const ProductionAlg &getProductionAlg(int index)`：获取编号为`index`的产生式
- `const ElementDict &getDict()`：获取语法解析器的字典

`ElementDict`类：

- `int findElem(const std::string &elemName)`：获取名称为`elemName`的非终结符的编号
- `int findElem(const std::string &elemType, const std::string &elemVal)`：获取类型为`elemType`，值为`elemVal`的终结符的编号
- `AlgElement *getElem(int index)`：获取字典中编号为`index`的符号
- `EMPTYWORDID`：空字的编号
- `ENDCH`：结束符的编号

`AlgElement`类：

- `int getId()`：获取符号的编号

`LRChart`类：

- `const LRItem &get(int state, int ch)`：获取状态编号`state`接受编号为`ch`的符号时的动作

TODO:

<ul>
  <li>确定输入格式并编写输入处理代码</li>
  <li>词法分析器的接入：在语法分析过程中调用</li>
  <li>完成根据LR分析表进行语法分析的代码（实现LR1Main函数）</li>
  <li>中间代码生成逻辑（等待后续课程）</li>
  <li>语法分析错误处理逻辑</li>
</ul>

<br>

<h1>语法输入规则</h1>

多行输入，每一行的格式为：`[非终结符] -> [产生式]`

第一行输入左侧的非终结符认为是语法的起始符，该非终结符可以包含多个候选式，程序会自动添加一个新的起始符

非终结符的输入：非终结符是字母、数字或下划线构成的符号串，且必须以字母或数字开头

终结符的输入：根据词法分析的输出，按照 `{ 类型 值 }` 的规则进行输入，可以输入 `{ }`  表示空字，可以输入 `{ 类型 }` 表示接受该类型任意值的符号

产生式：单个终结符或非终结符构成产生式；产生式用空格连接产生式构成产生式；产生式使用候选式符号 `|`  连接产生式构成产生式。产生式的每个元素之间<strong>必须用空格分隔</strong>

转义符：`\` 可以对空格、左右大括号、`|` 和 `\` 进行转义，其余符号跟在转移符后含义不变

以下为一行示例输入：

`A -> { letter a } A | { operator \} } | { }`
