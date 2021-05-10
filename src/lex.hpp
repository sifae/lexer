#ifndef LEX_HPP
#define LEX_HPP

#define MAX_BUFSIZE 128

enum LexType
{
	Lplus, Lminus, Lmult, Ldiv, Lmod,
	EQ, LT , GT,
	Land, Lor, Lnot,
	Lsemi, Lcomm, 
  Llbrace, Lrbrace, Llsqbr, Lrsqbr,
  Lnum, Lstring, Lid, Lassign,
	Lif, Lthen, Lwhile, Ldo, Lgoto,
	Lprint,
	Lbuy, Lsell, Lprod, Lbuild, Lendturn,
  Lempty, Lerr //Lerr - no matching type was found 
};

int isoperator(char chr);

class Lexem
{
	LexType type;
	char    *str_lex;
	int     line;
public:
	Lexem(const char *str = "\0", LexType t = Lempty, int line = 0);
	~Lexem();
	LexType getType();
	char    *getStr();
	int     getLine();
};

class Scanner
{
	enum State {H, N, I, K, A, S, Err} state;
  char    chr;
  int     line;             //Line of code(by order) we parsing
	char    buf[MAX_BUFSIZE], *buf_top;
	Lexem   *lex; //Current lexeme, equals NULL if not finished yet
public:
	Scanner();
	Lexem *next(char chr); //Null if not finished
private:
  void run();
  void saveChr();
  void setState(State state);
  void endLex(LexType type);
	void analyze_H();
	void analyze_N();
	void analyze_I();
	void analyze_K();
	void analyze_A();
	void analyze_S();
	void analyze_D();
  LexType bufKeywordType();
  LexType bufOperatorType();
};

#endif
