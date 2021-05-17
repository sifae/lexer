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
  sprintf(tmp, " at line %d", lex->getLine()-1);
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

int Analyzer::isOperator()
{
  const int math = 
    currentType(Lplus) || currentType(Lminus) ||
    currentType(Lmult) || currentType(Ldiv)   ||
    currentType(Lmod); 
  const int logic = 
    currentType(Land) || currentType(Lor) || currentType(Lnot);
  const int comp = 
    currentType(LT) || currentType(GT) || currentType(EQ);
  return math || logic || comp;
}

int Analyzer::isLegitFunc()
{
  return isNoArgFunc() || isOneArgFunc(); 
}

int Analyzer::isNoArgFunc()
{
  const char table[8][32] = 
  { {"?my_id"},{"?turn"},{"?players"},{"?active_players"},
    {"?supply"},{"?raw_price"},{"?demand"},{"?production_price"}
  };
  for(auto fun : table){
    if(strcmp(fun, lex->getStr()) == 0)
      return 1;
  }
  return 0;
}

int Analyzer::isOneArgFunc()
{
  const char table[10][32] = 
  { {"?money"},{"?raw"},{"?production"},{"?factories"},
    {"?auto_factories"},{"?manufactured"},{"?result_raw_sold"},
    {"?result_raw_price"},{"?result_prod_bought"},
    {"?result_prod_price"}
  };
  for(auto fun : table){
    if(strcmp(fun, lex->getStr()) == 0)
      return 1;
  }
  return 0;
}

void Analyzer::Statement()
{
  if(lexs->next == NULL)
    return;
  If();
  While();
  Goto();
  Assign();
  Label();
  Print();
  EndTurn();
  Prod();
  Build();
  Buy();
  Sell();
  Statement();
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
  if(isOperator()){
    nextLex();
    Expression();
  }
}

//void Analyzer::OperatorLogic()
//{
//  if(currentType(Land)){
//    nextLex();
//    Expression();
//  } else if(currentType(Lor)){
//    //TODO
//  } else if(currentType(Lnot)){
//    //TODO
//  }
//}
//
//void Analyzer::OperatorMath()
//{
//  if(currentType(Lplus)){
//    //TODO
//  } else if(currentType(Lminus)){
//    //TODO
//  } else if(currentType(Lmult)){
//    //TODO
//  } else if(currentType(Ldiv)){
//    //TODO
//  } else if(currentType(Lmod)){
//    //TODO
//  }
//}
//
//void Analyzer::OperatorComp()
//{
//  if(currentType(LT)){
//    //TODO
//  } else if(currentType(GT)){
//    //TODO
//  } else if(currentType(EQ)){
//    //TODO
//  }
//}

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
    if(!isLegitFunc())
      throw SyntaxError("Function '%s' is not defined", lex, lex->getStr());
    else if(isNoArgFunc()){
      const char *name = lex->getStr();
      nextLex();
      if(currentType(Llbrace))
        throw SyntaxError("Function '%s' takes no arguments", lex, name);
    }
    else if(isOneArgFunc()){
      nextLex();
      ExpressionParenth(lex->getStr());
    }
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
  nextLex();
  Expression();
  if(!currentType(Lrbrace))
    throw SyntaxError("Expected ')' before '%s'",
          lex, lex->getStr()); 
  nextLex();
}

void Analyzer::If()
{
  if(currentType(Lif)){
    nextLex();
    ExpressionParenth("if");
    if(!currentType(Lthen))
      throw SyntaxError("Expected 'then' after expression", lex); 
    nextLex();
    Statement();
  }
}

void Analyzer::While()
{
  if(currentType(Lwhile)){
    nextLex();
    ExpressionParenth("while");
    Statement();
  }
}

void Analyzer::Label()
{
  if(currentType(Llabel)){
    nextLex();
    Statement();
  }
}

void Analyzer::Goto()
{
  if(currentType(Lgoto)){
    nextLex();
    if(!currentType(Llabel))
      throw SyntaxError("Expected label after 'goto'", lex);
    nextLex();
    if(!currentType(Lsemi))
      throw SyntaxError("Expected ';' after goto statement", lex);
    nextLex();
  }
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
  if(currentType(Lvar)){
    nextLex();
    Var();
    if(!currentType(Lassign))
      throw SyntaxError("Expected ':=' in declaration", lex);
    nextLex();
    Expression();
    if(!currentType(Lsemi))
      throw SyntaxError("Expected ';' after assignment", lex);
    nextLex();
  }
}

void Analyzer::Print()
{
  if(currentType(Lprint)){
    nextLex();
    PrintList();
    if(!currentType(Lsemi))
      throw SyntaxError("Expected ';' after print statement", lex);
    nextLex();
  }
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
  if(currentType(Lendturn)){
    nextLex();
    if(!currentType(Lsemi))
      throw SyntaxError("Expected ';' after 'endturn'", lex);
    nextLex();
  }
}

void Analyzer::Prod()
{
  if(currentType(Lprod)){
    nextLex();
    Expression();
    if(!currentType(Lsemi))
      throw SyntaxError("Expected ';' after prod statement", lex);
    nextLex();
  }
}

void Analyzer::Build()
{
  if(currentType(Lbuild)){
    nextLex();
    Expression();
    if(!currentType(Lsemi))
      throw SyntaxError("Expected ';' after build statement", lex);
    nextLex();
  }
}

void Analyzer::Buy()
{
  if(currentType(Lbuy)){
    nextLex();
    Expression();
    if(!currentType(Lcomm))
      throw SyntaxError("Expected ',' after 1 argument in 'buy'", lex);
    nextLex();
    Expression();
    if(!currentType(Lsemi))
      throw SyntaxError("Expected ';' after buy statement", lex);
    nextLex();
  }
}

void Analyzer::Sell()
{
  if(currentType(Lsell)){
    nextLex();
    Expression();
    if(!currentType(Lcomm))
      throw SyntaxError("Expected ',' after 1 argument in 'sell'", lex);
    nextLex();
    Expression();
    if(!currentType(Lsemi))
      throw SyntaxError("Expected ';' after sell statement", lex);
    nextLex();
  }
}
