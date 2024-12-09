# Compiler-Principle

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
