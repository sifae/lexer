#include <string.h>
#include "lex.hpp"

struct LexTripl{
  const char *str;
  int str_len;
  LexType type;
};

int isdigit(char chr)
{
  return chr >= '0' && chr <= '9';
}

int isletter(char chr)
{
  return (chr >= 'a' && chr <= 'z') || (chr >= 'A' && chr <= 'Z');
}

int isendlex(char chr)
{
  return chr==' ' || chr=='\n' || chr=='\t'; 
}

int isoperator(char chr)
{
  return strchr("+-*/%<>=:,;()[]", chr) != NULL;
}

Lexem::Lexem(const char *str, LexType t, int line_)
{
	str_lex = new char[strlen(str)+1];
	strcpy(str_lex, str);
	type = t;
	line = line_;
}

Lexem::~Lexem()
{
	delete[] str_lex;
}

LexType Lexem::getType()
{
	return type;
}

char *Lexem::getStr()
{
	return str_lex;
}

int Lexem::getLine()
{
	return line;
}

void Scanner::saveChr()
{
  *buf_top = chr; 
  buf_top++;
}

Lexem *Scanner::next(char chr_)
{ 
  //Ignore new data if error occured
  if(state != Err)
    chr = chr_;
  run();  
  return lex;
}

void Scanner::run()
{
	switch(state){
    case H:
      analyze_H();
      break;
    case N:
      analyze_N();
      break;
    case I:
      analyze_I();
      break;
    case K:
      analyze_K();
      break;
    case A:
      analyze_A();
      break;
    case S:
      analyze_S();
      break;
    case Err:
      break;
  }
}

void Scanner::setState(State state_)
{
    state = state_;
    lex = NULL;
    saveChr();
}

void Scanner::endLex(LexType type)
{
  state = H;
  *buf_top = '\0';
  lex = new Lexem(buf, type, line);
  if(chr == '\n')
    line++;
  memset(buf, '\0', MAX_BUFSIZE);
  buf_top = buf;
}

LexType Scanner::bufKeywordType()
{
  const LexTripl table[11] =
  { {"if",    2, Lif},    {"do",    2, Ldo},
    {"then",  4, Lthen},  {"sell",  4, Lsell},
    {"goto",  4, Lgoto},  {"prod",  4, Lprod},
    {"print", 5, Lprint}, {"build", 5, Lbuild},
    {"while", 5, Lwhile}, {"endturn",  4, Lendturn},
    {"buy", 3, Lbuy} };
  for(LexTripl t : table){
    if(memcmp(t.str, buf, t.str_len) == 0)
      return t.type;
  }
  return Lerr;
}

LexType Scanner::bufOperatorType()
{
  const LexTripl table[17] =
  { {"+", 1, Lplus},  {">", 1, GT},
    {"-", 1, Lminus}, {"=", 1, EQ},
    {"*", 1, Lmult},  {"<", 1, LT},
    {"/", 1, Ldiv},   {"&", 1, Land},
    {"%", 1, Lmod},   {"|", 1, Lor},
    {";", 1, Lsemi},  {",", 1, Lcomm},
    {"(", 1, Llbrace},{"[", 1, Llsqbr},
    {")", 1, Lrbrace},{"]", 1, Lrsqbr},
    {"!", 1, Lnot} };
  for(LexTripl t : table){
    if(memcmp(t.str, buf, t.str_len) == 0)
      return t.type;
  }
  return Lerr;
}

Scanner::Scanner()
{
	state = H;
	line = 1;
  buf_top = buf;
}

void Scanner::analyze_H()
{
  if(isendlex(chr)){
    lex = NULL;
    if(chr == '\n') 
      line++;
    return;
  } 
  if(isdigit(chr)){
    setState(N);
    return;
  } 
  if(strchr("?@$", chr)){
    setState(I);
    return;
  } 
  if(isletter(chr)){
    setState(K);
    return;
  } 
  if(chr == ':'){
    setState(A);
    return;
  } 
  if(chr == '"'){
    setState(S);
    return;
  }
  if(isoperator(chr)){ //Double check ':' takes place
    saveChr();
    LexType type = bufOperatorType();
    if(type != Lerr){
      endLex(type);
      return;
    }
  }
  state = Err;
}

void Scanner::analyze_N()
{
  if(isendlex(chr) || isoperator(chr)){
    endLex(Lnum);
    return;
  } 
  if(isdigit(chr)){
    setState(N);
    return;
  } 
  state = Err;
}

void Scanner::analyze_I()
{
  if(isendlex(chr) || isoperator(chr)){
    endLex(Lid);
    return;
  }
  if(isletter(chr) || isdigit(chr) || chr == '_'){
    setState(I);
    return;
  }
  state = Err;
}

void Scanner::analyze_K()
{
  if(isendlex(chr) || isoperator(chr)){
    LexType type = bufKeywordType();
    if(type != Lerr){
      endLex(type);
      return;
    }
  }
  if(isletter(chr)){
    setState(K);
    return;
  }
  state = Err;
}

void Scanner::analyze_A()
{
	if(chr == '='){
    saveChr();
    endLex(Lassign);
    return;
	}
  state = Err;
}

void Scanner::analyze_S()
{
	if(chr == '"'){
    saveChr();
    endLex(Lstring);
    return;
	}
  setState(S);
}
