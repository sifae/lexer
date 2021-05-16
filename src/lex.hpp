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
  Lempty, Lnomatch, Lerr
};

class Lexem
{
	LexType type;
	char    *str_lex;
	int     line;
public:
	Lexem(LexType t = Lempty, int line = 0, const char *str = "\0");
	~Lexem();
	LexType getType();
	char    *getStr();
	int     getLine();
};

struct LexLst{
  Lexem *lex;
  LexLst *next;
};

class Scanner
{
	enum State {H, N, I, K, A, S, Err} state;
  char    chr;
  int     line; //Line of code(by order) we parsing
	char    buf[MAX_BUFSIZE], *buf_it;
	Lexem   *lex; //Current lexeme, equals NULL if not finished yet
  LexLst  *lexs, *lexs_it;
public:
	Scanner();
  ~Scanner();
	void next(char chr);
  LexLst  *getLexs();   //Returns NULL if error occured
  Lexem   *getError();
private:
  void run();
  void saveChr();
  void setState(State state);
  void endLex(LexType type);
  void appendLex();
  void step();  //Construct list of lexems 
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
