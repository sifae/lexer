#include <string.h>
#include <stdio.h>
#include "lexer.hpp"
#include "syntax.hpp"

int main(int argc, char *argv[])
{
  LexLst *lexs;
  int c;
  FILE *file;
  Scanner scan;
  
  if(argc < 2){
    fprintf(stderr, "Filename required\n");
    return 1;
  }

  file = fopen(argv[1], "r");
  if(!file){
    fprintf(stderr, "Cannot open file\n");
    return 1;
  }

  while((c = fgetc(file)) != EOF)
    scan.next(c);

  lexs = scan.getLexs();
  if(!lexs){
    Lexem errLex = *scan.getError();
    printf("Lexical Error: Unexpected '%s' at line %d\n", 
        errLex.getStr(), errLex.getLine());
    return 1;
  }
  Analyzer analyzer(lexs);
  try{
    analyzer.run();
  } catch(SyntaxError &e){
    printf("Syntax Error: %s\n", e.getMsg());
    return 1;
  } 
  return 0;
}
