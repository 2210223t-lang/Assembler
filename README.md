[![Language](https://img.shields.io/badge/language-C-blue.svg)](https://en.wikipedia.org/wiki/C_(programming_language))

[![GitHub forks](https://img.shields.io/github/forks/2210223t-lang/2-linked-list)](https://github.com/2210223t-lang/2-linked-list/network/members)
[![GitHub stars](https://img.shields.io/github/stars/2210223t-lang/2-linked-list)](https://github.com/2210223t-lang/2-linked-list/stargazers)
[![GitHub pull requests](https://img.shields.io/github/issues-pr/2210223t-lang/2-linked-list)](https://github.com/2210223t-lang/2-linked-list/pulls)
[![GitHub last commit](https://img.shields.io/github/last-commit/2210223t-lang/2-linked-list)](https://github.com/2210223t-lang/2-linked-list/commits/main)


# Assembler
Repo provide user with tools for a custom stack-based assembler language.
Pequliarity of this assmebler is a huge variety of usefull features, which can be used in different situations, supporting developers in every kind of situation.


## Possible usage
Repo contains strong debugging tool, which is an amazing example for a beginner to stick to.


## Tech

### Structure
```spu
|
├── execute.cpp <-- runs programm
|
├── assembler <-- contains assembling func
|   |
|   ├── arguments.h   <--- Parsing arguments functions
|   ├── arguments.cpp
|   |
|   ├── compiler.cpp  <--- Compiling func
|   ├── compiler.h
|   |
|   ├── labels.cpp    <--- Working with labels
|   └── labels.h
|
|
├── disassembler <--- Disassembling func
|   |
|   ├── disasm.cpp <--- Disasembling func
|   ├── disasm.h
|
|
├── processor <--- SPU functions
|   |
|   ├── asm_utility.cpp <--- Utility functions + config
|   ├── asm_utility.h
|   |
|   ├── calc.cpp        <--- Processor main functions
|   ├── calc.h
|   |
|   ├── commands.cpp    <--- All available commands realisation
|   ├── commands.h
|
|
└── code <--- Service folder\
    |
    ├── byte_code.txt <--- Compiled byte code
    |
    └── list_code.txt <--- List of compiled code

```


### Commands
All available commands

1.  SQRT - Std sqrt func
2.  JMPN - Jumps to an argument if last calculated value was negative
3.  HLT - Exit
4.  PUSH - Pushes argument into stack
5.  ADD - Summarizes last 2 elements of stack
6.  DIV - Divides last 2 elemensts of stack
7.  OUT - Prints last element of stack
8.  POP - Pops last element of stack ( into argument ( register of memory adress ) )
9.  SUB - Subtract last 2 elements of stack
10. FACT - Calculates factorial of last element in stack ( if decimal )
11. MULT - Multiplies last 2 elements of stack
12. REST - Cleans stack values
13. JMP - Jumps into argument adress
14. SQR - Calculates square root of last element in stack
15. CALL - Calls function, specified in argument
16. RET - Returns to place, where were called( for functions )
17. JMPP - Jumps to an argument if last calculated value was positive
18. JMPZ - Jumps to an argument if last calculated value was zero


### Registers
Names, which user can push and pop in'n out

Example of usage:

```PUSH RAX```

1.  REX
2.  RAX
3.  RBX
4.  RCX
5.  RDX
6.  RSI
7.  RDI
8.  RSP
9.  RBP
10. RCP


# Press F to pay respect

[Нетреба Николаю](https://github.com/NikolayNetreba) за помощь в тестировании при разработки этого проекта

[Деду](https://soundcloud.com/ilya-dedinsky-619322134?utm_source=clipboard&utm_medium=text&utm_campaign=social_sharing)( ┌(・o・)┐  HELP )
