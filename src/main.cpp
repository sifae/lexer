#include <iostream>
#include <string.h>
#include "lex.hpp"

std::ostream &operator<<(std::ostream &os, Lexem &lex)
{
      os << "Lexem: " << lex.getStr()   << 
            " Type: " << lex.getType()  << 
            " Line: " << lex.getLine();
      return os;
}

int main(int argc, char *argv[])
{
	Lexem *lex;
	char c;
	FILE *file;
	Scanner scan;

	file = fopen(argv[1], "r");
  if(!file)
    fprintf(stderr, "Cannot open file\n");
	while((c = fgetc(file)) != EOF){
    lex = scan.next(c);
    if(lex){
      if(isoperator(c) && !strchr(lex->getStr(), c)){
        std::cout << *lex << '\n';
        lex = scan.next(c);
      }
      if(lex) //lex can ne NULL if c is ':'
        std::cout << *lex << '\n'; 
    }
	}
	return 0;
}
