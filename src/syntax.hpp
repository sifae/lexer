#ifndef SYNTAX_HPP
#define SYNTAX_HPP

#include"lexer.hpp"

#define MSG_SIZE 128

class Analyzer
{
	Lexem *lex;
  LexLst *lexs;
public:
	Analyzer(LexLst *lexs);
	void run();
private:
  void nextLex();
  void Expression();
  void ExpressionParenth(const char *ExprName);
  void Statement();
  void If();
  void While();
  void Label();
  void Goto();
  void Var();
  void Assign();
  void Print();
  void PrintList();
  void PrintElem();
  void EndTurn();
  void Prod();
  void Build();
  void Buy();
  void Sell();
  void Operator();
  //void OperatorLogic();
  //void OperatorMath();
  //void OperatorComp();
  void NumConst();
  void Function();
  void VarExpr();
  int currentType(LexType type);
  int isOperator();
  int isLegitFunc();
  int isOneArgFunc();
  int isNoArgFunc();
};

class SyntaxError
{
  char msg[MSG_SIZE];
public:
  SyntaxError(const char *msg, Lexem *lex, ...);
  const char *getMsg();
};

#endif
