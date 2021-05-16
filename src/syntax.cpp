#include<string.h>
#include<stdarg.h>
#include<stdio.h>
#include"lexer.hpp"
#include"syntax.hpp"

SyntaxError::SyntaxError(const char *fmt, Lexem *lex, ...)
{
  char tmp[MSG_SIZE];
  va_list args;

  va_start(args, lex);
  vsprintf(msg, fmt, args);
  sprintf(tmp, " at line %d", lex->getLine());
  va_end(args);

  strcat(msg, tmp);
}

const char *SyntaxError::getMsg()
{
  return msg;
}

Analyzer::Analyzer(LexLst *lexs_)
{
  lexs = lexs_;
}

void Analyzer::run()
{
  lex = lexs->lex;
  Statement();
}

void Analyzer::nextLex()
{
  lexs = lexs->next;
  lex = lexs->lex;
}

int Analyzer::currentType(LexType type)
{
  return lex->getType() == type;
}

//int Analyzer::isOperator()
//{
//  const int flag = 
//    currentType(Lplus) || currentType(Lminus) ||
//    currentType(Lmult) || currentType(Ldiv)   ||
//    currentType(Lmod)  || currentType(Land)   ||
//    currentType(Lor)   || currentType(Lnot);
//  return flag;
//}

void Analyzer::Statement()
{
  if(currentType(Lif)){
    nextLex();
    If();
  } else if(currentType(Lwhile)){
    nextLex();
    While();
  } else if(currentType(Lgoto)){
    nextLex();
    Goto();
  } else if(currentType(Lvar)){
    nextLex();
    Assign();
  } else if(currentType(Llabel)){
    nextLex();
    Label();
  } else if(currentType(Lprint)){
    nextLex();
    Print();
  } else if(currentType(Lendturn)){
    nextLex();
    EndTurn();
  } else if(currentType(Lprod)){
    nextLex();
    Prod();
  } else if(currentType(Lbuild)){
    nextLex();
    Build();
  } else if(currentType(Lbuy)){
    nextLex();
    Buy();
  } else if(currentType(Lsell)){
    nextLex();
    Sell();
  }
  else 
      throw SyntaxError("Something went wrong", lex);
}

void Analyzer::Expression()
{
  Operator();
  NumConst();
  Function();
  VarExpr();
}

void Analyzer::Operator()
{
  OperatorLogic();
  OperatorMath();
  OperatorComp();
}

void Analyzer::OperatorLogic()
{
  if(currentType(Land)){
    //TODO
  } else if(currentType(Lor)){
    //TODO
  } else if(currentType(Lnot)){
    //TODO
  }
}

void Analyzer::OperatorMath()
{
  if(currentType(Lplus)){
    //TODO
  } else if(currentType(Lminus)){
    //TODO
  } else if(currentType(Lmult)){
    //TODO
  } else if(currentType(Ldiv)){
    //TODO
  } else if(currentType(Lmod)){
    //TODO
  }
}

void Analyzer::OperatorComp()
{
  if(currentType(LT)){
    //TODO
  } else if(currentType(GT)){
    //TODO
  } else if(currentType(EQ)){
    //TODO
  }
}

void Analyzer::NumConst()
{
  if(currentType(Lnum)){
    nextLex();
    Expression();
  }
}

void Analyzer::Function()
{
  if(currentType(Lfunc)){
    //TODO Handle external functions
    nextLex();
    Expression();
  }
}

void Analyzer::VarExpr()
{
  if(currentType(Lvar)){
    nextLex();
    Var();
    Expression();
  }
}

void Analyzer::ExpressionParenth(const char *ExprName)
{
  if(!currentType(Llbrace))
    throw SyntaxError("Expected '(' after '%s'", lex, ExprName); 
  Expression();
  if(!currentType(Lrbrace))
    throw SyntaxError("Expected ')' before '%s'",
          lex, lexs->next->lex->getStr()); 
  nextLex();
}

void Analyzer::If()
{
  ExpressionParenth("if");
  if(!currentType(Lthen))
    throw SyntaxError("Expected 'then' after expression", lex); 
  nextLex();
  Statement();
}

void Analyzer::While()
{
  ExpressionParenth("while");
  Statement();
}

void Analyzer::Label()
{
  nextLex();
  Statement();
}

void Analyzer::Goto()
{
  if(!currentType(Llabel))
    throw SyntaxError("Expected label after 'goto'", lex);
  nextLex();
  if(!currentType(Lsemi))
    throw SyntaxError("Expected ';' after goto statement", lex);
  nextLex();
}

void Analyzer::Var()
{
  if(currentType(Llsqbr)){
    nextLex();
    Expression();
    if(!currentType(Lrsqbr))
      throw SyntaxError("Expected ']' in array access statement", lex); 
    nextLex();
  }
}

void Analyzer::Assign()
{
  Var();
  if(!currentType(Lassign))
    throw SyntaxError("Expected ':=' after variable declaration", lex);
  nextLex();
  Expression();
  if(!currentType(Lsemi))
    throw SyntaxError("Expected ';' after assignment", lex);
  nextLex();
}

void Analyzer::Print()
{
  PrintList();
  if(!currentType(Lsemi))
    throw SyntaxError("Expected ';' after print statement", lex);
  nextLex();
}

void Analyzer::PrintList()
{
  PrintElem();
  if(currentType(Lcomm)){
    nextLex();
    PrintList();
  } 
  nextLex();
}

void Analyzer::PrintElem()
{
  if(currentType(Lstring)){
    nextLex();
  } else 
    Expression();
}

void Analyzer::EndTurn()
{
  if(!currentType(Lsemi))
    throw SyntaxError("Expected ';' after 'endturn'", lex);
  nextLex();
}

void Analyzer::Prod()
{
  Expression();
  if(!currentType(Lsemi))
    throw SyntaxError("Expected ';' after prod statement", lex);
  nextLex();
}

void Analyzer::Build()
{
  Expression();
  if(!currentType(Lsemi))
    throw SyntaxError("Expected ';' after build statement", lex);
  nextLex();
}

void Analyzer::Buy()
{
  Expression();
  if(!currentType(Lcomm))
    throw SyntaxError("Expected ',' after 1 argument in 'buy'", lex);
  nextLex();
  Expression();
  if(!currentType(Lsemi))
    throw SyntaxError("Expected ';' after buy statement", lex);
}

void Analyzer::Sell()
{
  Expression();
  if(!currentType(Lcomm))
    throw SyntaxError("Expected ',' after 1 argument in 'sell'", lex);
  nextLex();
  Expression();
  if(!currentType(Lsemi))
    throw SyntaxError("Expected ';' after sell statement", lex);
}
