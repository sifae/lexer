#include <string.h>
#include <stdio.h>
#include "lex.hpp"

int main(int argc, char *argv[])
{
	LexLst *lexs;
	int c;
	FILE *file;
	Scanner scan;

	file = fopen(argv[1], "r");
  if(!file)
    fprintf(stderr, "Cannot open file\n");

	while((c = fgetc(file)) != EOF)
    scan.next(c);

  lexs = scan.getLexs();
  if(!lexs){
    Lexem errLex = *scan.getError();
    printf("Unexpected '%s' at line %d\n", 
        errLex.getStr(), errLex.getLine());
    return 1;
  }
  for(auto it = lexs; it->next != NULL; it = it->next) 
    printf("Lexem: %s Type: %d Line: %d\n", 
        it->lex->getStr(), it->lex->getType(), it->lex->getLine());
	return 0;
}
