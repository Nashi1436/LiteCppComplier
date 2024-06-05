# README_lex

[项目地址←](https://github.com/Nashi1436/LiteCppComplier)

[文档在线地址←](https://www.notion.so/README-63b502ee57754ba5a307823d9ca2a621?pvs=21)

## 描述

这里是第一节词法分析的部分

## 相关文件

`p1-p3.cpp`为各部分代码

`.in`  `.out` 为各部分输入输出样例

`lex.cpp` 为综合词法分析代码

## 任务

### part1

【任务介绍】在词法分析之前，对程序员提交的源程序进行预处理，剔除注释等不必要的字符，以简化词法分析的处理

【输入】字符串形式的源程序。

【输出】处理之后的字符串形式的源程序。

【题目】设计一个程序，从任意字符串中剔除C语言形式的注释，包括：

1. 形如： //…的单行注释；
2. 形如： /*…*/ 的多行注释。

### part2

【任务介绍】根据给定源语言的构词规则，从任意字符串中识别出所有的合法标识符。

【输入】字符串。

【输出】单词符号流，一行一个单词。

【题目】设计一个程序，从任意字符串中识别出所有可视为C语言“名字”的子串。注意：

1. 构词规则：以字母打头，后跟任意多个字母、数字的单词；长度不超过15；不区分大小写；把下划线 视为第27个字母。
2. 关键字保留，即：语言定义中保留了某些单词用作关键字，同学们不可以将这些单词用作“名字“（变量名、常量名、函数名、标号名等等）。

### part3

【任务介绍】根据给定源语言的构词规则，从任意字符串中识别出该语言所有的合法的单词符号，并以等长的二元组形式输出。

【输入】字符串形式的源程序。

【输出】单词符号所构成的串（流），单词以等长的二元组形式呈现。

【题目】设计一个程序，根据给定源语言的构词规则，从任意字符串中识别出该语言所有的合法的单词符号，并以等长的二元组形式输出

# lex特性

处理源代码 去掉注释 `//` `/**/` 与宏定义 强制字母小写
得到token列表

限制处理代码长度 `1e6` 

暂未实现手写处理 `正则表达式` → `化简DFA`

## 命令行控制

使用命令参数控制输入输出文件

输出参数不可超过: `20argc` 

默认文件输出输出: `对应文件名.in`  `对应文件名.out` 

**运行命令格式 `./lex SouceFileName (-ParaName FileName)*`**

### 示例命令

```jsx
g++ lex.cpp -o lex
./lex
./lex lex.in。/
./lex lex.in -o lex.out
./lex -o lex.out lex.in 
```

![Untitled](README_lex%20ab5bb72fb2704bf0a17e4793a5ade79f/Untitled.png)

## 错误的行列定位及报错

暂未支持打印错误所在行以及栈回溯

![Untitled](README_lex%20ab5bb72fb2704bf0a17e4793a5ade79f/Untitled%201.png)

![Untitled](README_lex%20ab5bb72fb2704bf0a17e4793a5ade79f/Untitled%202.png)

![Untitled](README_lex%20ab5bb72fb2704bf0a17e4793a5ade79f/Untitled%203.png)

## 根据表获取token

减少了手动处理的繁琐
暂未支持宏定义的处理
暂未支持`0x3f` `0.24lf` `1.2e9`  等的数字表示方法
注释与引号为单独处理

![Untitled](README_lex%20ab5bb72fb2704bf0a17e4793a5ade79f/Untitled%204.png)

# 代码

## 主函数

分为  `处理编译命令`  `文件操作`  `主处理`  `结束`  四个部分

![Untitled](README_lex%20ab5bb72fb2704bf0a17e4793a5ade79f/Untitled%205.png)

## 去注释

获取`string格式`代码，从前往后处理

每次单独处理完 一整部分 `//`   `/**/`  `“”`

![Untitled](README_lex%20ab5bb72fb2704bf0a17e4793a5ade79f/Untitled%206.png)

## 强制代码小写

![Untitled](README_lex%20ab5bb72fb2704bf0a17e4793a5ade79f/Untitled%207.png)

## 获取token

类似处理注释，每次处理一整块部分

`String`  `Numbers`  `Keywords`  `Identifiers`  `Operators`  `delimiters`  `define` 

处理符号时 每次从长度为3依次减少，检查map中是否存在对应符号

![Untitled](README_lex%20ab5bb72fb2704bf0a17e4793a5ade79f/Untitled%208.png)