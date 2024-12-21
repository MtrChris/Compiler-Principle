# Commitment

在完成 DFA 化简任务的过程中，我做了以下的修改：

1. 将报错的 throw excption 改为 ios_base::failure 和 runtime_error. (REtoNFA.cpp)

2. 将 printNFA 中的 “终态:” 改为 “Final State:”，“cout << (char)i;” 改为 “cout << char(i) << ' ';”. (LexTool.cpp)

3. 在 Lex.h 中添加了 SimplifyDFA.cpp 中函数的声明，并包含了头文件 algorithm. (Lex.h)

4. 添加 SimplifyDFA.cpp 文件，用于化简 DFA. (SimplifyDFA.cpp)

5. 向 README.md 中添加了对 DFA 化简这一步骤的测试结果. (README.md)

6. 遇见中文乱码时，尝试用 GB2312 重新打开