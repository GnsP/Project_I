/******************************************************************************

	 PROTOTYPE OF I PROGRAMMING LANGUAGE INTERPRETATION SYSTEM VERSION-3

******************************************************************************/

#ifndef LEXER_H_
#define LEXER_H_


#include "includes.hpp"
using namespace std;

bit64 line_no;
bit64 col_no;

class I_error{
	public:
		I_error(const char *msg):
			error_message_(msg), line_no_(line_no), col_no_(col_no) {}
			
		void print(){
			cout<<"ERROR :: LINE="<<line_no_<<" CHARACTER="<<col_no_
				<<" :: "<<error_message_<<endl;
			exit(0);
		}
		
	private:
		const char *error_message_;
		int line_no_;
		int col_no_; 
};


enum TokenType{ SYMBOL = 0, INTEGER, REAL, OPERATOR, STRING , _NONE };
const char *typeString(TokenType t){
	switch(t){
		case SYMBOL:
			return "SYMBOL";
		case INTEGER:
			return "INTEGER";
		case REAL:
			return "REAL";
		case OPERATOR:
			return "OPERATOR";
		case STRING:
			return "STRING";
		case _NONE:
			return "_NONE";
		default:
			return "ERROR ...";
	}
}

enum Operator{	PLUS, MINUS, MULT, DIV, MOD, POW,
				AND, OR, NOT, XOR,
				GT, LT, GTE, LTE, EQ, NEQ , 
				LPAREN, RPAREN, ERROR, 
				ESCAPE, ASSIGN, LCURL, RCURL, COMMA };

const char *opString(Operator op){
	switch(op){
		case PLUS:  return "PLUS";
		case MINUS: return "MINUS";
		case MULT:  return "MULT";
		case DIV:   return "DIV";
		case MOD:   return "MOD";
		case POW:   return "POW";
		case AND:   return "AND";
		case OR:    return "OR";
		case NOT:   return "NOT";
		case XOR:   return "XOR";
		case GT:    return "GT";
		case LT:    return "LT";
		case GTE:   return "GTE";
		case LTE:   return "LTE";
		case EQ:    return "EQ";
		case NEQ:   return "NEQ";
		case LPAREN:return "LPAREN";
		case RPAREN:return "RPAREN";
		case ERROR: return "ERROR";
		case ESCAPE:return "ESCAPE";
		case ASSIGN:return "ASSIGN";
		case LCURL:	return "LCURL";
		case RCURL: return "RCURL";
		case COMMA:	return "COMMA";
		default:    return "\0";
	}
}

union TokenHolder{
	int integer;
	double real;
	char str[1024];
	char sym[1024];
	Operator op;
};

Operator str2op(const char *str){
	switch(str[0]){
		case '+': return PLUS;
		case '-': return MINUS;
		case '*':
			if( str[1]=='*') return POW;
			return MULT;
		case '/': return DIV;
		case '%': return MOD;
		case '&': return AND;
		case '|': return OR;
		case '~':
			if( str[1]=='=') return NEQ;
			return NOT;
		case '^': return XOR;
		case '>':
			if( str[1]=='=') return GTE;
			return GT;
		case '<':
			if( str[1]=='=') return LTE;
			return LT;
		case '=': 
			if(str[1]=='=') return EQ;
			return ASSIGN;
		case '(': return LPAREN;
		case ')': return RPAREN;
		case '{': return LCURL;
		case '}': return RCURL;
		case '\\':return ESCAPE;
		case ',': return COMMA;
		default:
			return ERROR;
	}
}

class Token{
	public:

		Token():type_(_NONE){}
		Token(TokenType type, const char *tok_str){
			makeToken(type, tok_str);
		}

		Token (const Token &oldToken){
			type_ = oldToken.type_;
			token_ = oldToken.token_;
		}

		void print(){
			cout<<"< "<<typeString(type_)<<", ";
			switch(type_){
				case INTEGER:   cout<<token_.integer<<" >";           break;
				case REAL:      cout<<token_.real<<" >";              break;
				case SYMBOL:    cout<<token_.sym<<" >";               break;
				case STRING:    cout<<token_.str<<" >";               break;
				case OPERATOR:  cout<<opString(token_.op)<<" >";      break;
				case _NONE:     cout<<" >";                           break;
				default:        cout<<" >";                           break;
			}
		}

		void makeToken(TokenType type, const char *tok_str){
			type_ = type;
			switch(type_){
				case SYMBOL:
					strcpy( token_.sym, tok_str);
					break;
				case INTEGER:
					token_.integer = atoi(tok_str);
					break;
				case REAL:
					token_.real = atof(tok_str);
					break;
				case STRING:
					strcpy(token_.str, tok_str);
					break;
				case OPERATOR:
					token_.op = str2op(tok_str);
					break;
				case _NONE:
					break;
				default:
					cout<<"ERROR OCCURED IN LEXICAL ANALYSIS"<<endl;
					break;
			}
		}

		TokenType getType(){ return type_; }

		TokenType type_;
		TokenHolder token_;
};


class Tokenizer{
	public:
		Tokenizer():expr_(NULL),type_(_NONE){}
		Tokenizer(const char *expr):expr_(expr){}

		bool next(Token &t){
			bool ret = getNextToken();
			t.makeToken(type_, token_);
			return ret;
		}

		const char* getExpr(){ return expr_; }

	private:
		const char *expr_;
		char token_[1024];
		TokenType type_;

		bool isOperator(char ch){
			if(strchr("+-*/%^=(){}[]&|~><?:$@,\\", ch)) return true;
			return false;
		}

		bool getNextToken(){
			char *temp;
			type_ = _NONE;
			temp = token_;
			*temp = '\0';

			if(!*expr_) return false;
			while(isspace(*expr_)) {
				expr_++ ;
				col_no++;
			}

			if( isOperator(*expr_)){
				while(isOperator(*expr_)){ 
					*temp++ = *expr_++;
					col_no++;
				}
				type_ = OPERATOR;
			}
			else if(isalpha(*expr_)){
				while(isalpha(*expr_) || *expr_ == '_'){
					*temp++ = *expr_++;
					col_no++;
				}
				type_ = SYMBOL;
			}
			else if(isdigit(*expr_)){
				while(isdigit(*expr_)){
					*temp++ = *expr_++;
					col_no++;
				}
				if(*expr_ == '.'){
					*temp++ = *expr_++;
					col_no++;
					while(isdigit(*expr_)){
						*temp++ = *expr_++;
						col_no++;
					}
					type_ = REAL;
				}
				else type_ = INTEGER;
			}
			else if(*expr_ == '\"'){
				*expr_++;
				col_no++;
				while(*expr_ != '\"'){
					*temp++ = *expr_++;
					col_no++;
				}
				*expr_++;
				col_no++;
				type_ = STRING;
			}
			else{
				while(!isspace(*expr_)){
					*temp++ = *expr_++;
					col_no++;
				}
				type_ = SYMBOL;
			}

			*temp = '\0';
			return true;
		}
				
};

enum atomType{ TOKEN, SEXPR, LEXPR };
struct atom{
	atomType type_;
	void *atom_;
};


//-----------------------------------------------------------------------------
//		FORWARD DECLARATION OF L-EXPRESSION AND ITS PARSER FOR USE IN
//					CLASSES USED FOR PARSING S-EXPRESSIONS
//-----------------------------------------------------------------------------

int indentLevel;

struct Logical_not{
	vector<atom> logical_not_;		// Unary operator ~
	void print();
};

struct Logical_xor{
	vector<Logical_not> logical_xor_;	// values separated by ^
	void print();
};

struct Logical_and{
	vector<Logical_xor> logical_and_;	// values separated by &
	void print();
};

struct Logical_or{
	vector<Logical_and> logical_or_; // Values separated by |
	void print();
};

struct Unary{
	vector<Logical_or>	unary_;	// Unary operators +, -
	void print();
};

struct Exponent{
	vector<Unary> exponent_;	// Values separated by exponent op **
	void print();
};

struct Factor{					// Values separated by *, / and %
	vector<Exponent> factor_;
	void print();
};

struct Term{					//Values separated by + and -
	vector<Factor> term_;
	void print();
};

struct Component{				//The if-else operator _?_:_
	vector<Term> component_;
	void print();
};

struct LExpr{					//Comma separated values
	vector<Component> lexpr_;
	void print();
};


class Logical_notParser{
	public:
		Logical_notParser(const vector<Token> &tok_stream, size_t begin){
			for(size_t i=begin; i<tok_stream.size(); i++){
				tok_stream_.push_back(tok_stream[i]);
			}
		}
		Logical_notParser(const vector<Token> &tok_stream)
			:tok_stream_(tok_stream){}
		Logical_notParser(){}

		void construct(const vector<Token> &tok_stream, size_t begin){
			for(size_t i=begin; i<tok_stream.size(); i++){
				tok_stream_.push_back(tok_stream[i]);
			}
		}

		pair<Logical_not, size_t> getLogical_not();

	private:
		vector<Token> tok_stream_;
		
		size_t parse_(size_t begin);

};

class Logical_xorParser{
	public:
		Logical_xorParser(const vector<Logical_not> &tok_stream, size_t begin){
			for(size_t i=begin; i<tok_stream.size(); i++){
				tok_stream_.push_back(tok_stream[i]);
			}
		}
		Logical_xorParser(const vector<Logical_not> &tok_stream)
			:tok_stream_(tok_stream){}
		Logical_xorParser(){}

		void construct(const vector<Logical_not> &tok_stream, size_t begin){
			for(size_t i=begin; i<tok_stream.size(); i++){
				tok_stream_.push_back(tok_stream[i]);
			}
		}

		pair<Logical_xor, size_t> getLogical_xor();

	private:
		vector<Logical_not> tok_stream_;
		
		size_t parse_(size_t begin);

};

class LExprParser{
	public:
		LExprParser(const vector<Token> &tok_stream, size_t begin){
			for(size_t i=begin; i<tok_stream.size(); i++){
				tok_stream_.push_back(tok_stream[i]);
			}
		}
		LExprParser(const vector<Token> &tok_stream):tok_stream_(tok_stream){}
		LExprParser(){}

		void construct(const vector<Token> &tok_stream, size_t begin){
			for(size_t i=begin; i<tok_stream.size(); i++){
				tok_stream_.push_back(tok_stream[i]);
			}
		}

		pair<LExpr *,size_t> getLExpr();
		
	private:
		const char *expr_;
		vector<Token> tok_stream_;
		Token tok_;
		LExpr *lexpr_;
		bit32 curl_match_;
		bool comma_delimited_;

		size_t parse_(size_t begin, LExpr *lexpr);
};

//-----------------------------------------------------------------------------

struct SExpr{
	vector<atom> sexpr_;

	void print(){
		for(size_t i=0; i<sexpr_.size(); i++){
			if(sexpr_[i].type_ == TOKEN){
				Token *t = (Token *)sexpr_[i].atom_;
				t->print();
				cout<<" ";
			}
			else if(sexpr_[i].type_ == SEXPR){
				indentLevel++;
				cout<<"\n";
				for(int j=0;j<indentLevel;j++)cout<<"\t";cout<<"( ";
				SExpr *t = (SExpr *)sexpr_[i].atom_;
				t->print();
				indentLevel--;
				cout<<" )"<<endl;
				for(int j=0;j<indentLevel;j++)cout<<"\t";
			}
			else if(sexpr_[i].type_ == LEXPR){
				LExpr *l = (LExpr *)sexpr_[i].atom_;
				l->print();
			}
			else;
		}
	}
};

class SExprParser{
	public:
		SExprParser(const char *expr):expr_(expr),lex_(expr){
			while(lex_.next(tok_)) tok_stream_.push_back(tok_);
		}
		SExprParser(const vector<Token> &tok_stream, size_t begin){
			for(size_t i=begin; i<tok_stream.size(); i++){
				tok_stream_.push_back(tok_stream[i]);
			}
		}
		SExprParser(const vector<Token> &tok_stream):tok_stream_(tok_stream){}
		SExprParser(){}

		void construct(const vector<Token> &tok_stream, size_t begin){
			for(size_t i=begin; i<tok_stream.size(); i++){
				tok_stream_.push_back(tok_stream[i]);
			}
		}
		
		pair<SExpr *,size_t> getSExpr(){
			pair<SExpr *, size_t> ret;
			paren_match = 0;
			ret.first = new SExpr; 
			ret.second = parse_(0,ret.first);
			if(paren_match){
				I_error e("Parentheses mismatch");
				e.print();
			}
			return ret;
		}
	
	private:
		const char *expr_;
		vector<Token> tok_stream_;
		Token tok_;
		SExpr *sexpr_;
		Tokenizer lex_;
		bit32 paren_match;

		size_t parse_(size_t begin, SExpr *sexpr){
			size_t i;
			atom myAtom;
			SExpr *newSexpr;
			LExprParser lParser;
			pair<LExpr *,size_t> lres;
			for(i=begin; i<tok_stream_.size(); i++){
				if(tok_stream_[i].type_ == OPERATOR){
					switch(tok_stream_[i].token_.op){
						case LPAREN:	
							paren_match++;
							newSexpr = new SExpr;
							i = parse_(i+1, newSexpr);
							//atom myAtom;
							myAtom.type_=SEXPR;
							myAtom.atom_ = (void *)newSexpr;
							sexpr->sexpr_.push_back(myAtom);
							break;
						case RPAREN:
							paren_match--;
							return i;
						case LCURL:
							lParser.construct(tok_stream_, i+1);
							//i += lParser.parse_(0,);
							lres = lParser.getLExpr();
							i += lres.second;
							//atom myAtom;
							myAtom.type_ = LEXPR;
							myAtom.atom_ = (void *)lres.first;
							sexpr->sexpr_.push_back(myAtom);
							break;
						default:
							//atom myAtom;
							myAtom.type_=TOKEN;
							Token *myToken = new Token(tok_stream_[i]);
							myAtom.atom_ = (void *)myToken;
							sexpr->sexpr_.push_back(myAtom);
							break;
					}
				}
				else{
					//atom myAtom;
					myAtom.type_ = TOKEN;
					Token *myToken = new Token(tok_stream_[i]);
					myAtom.atom_ = (void *)myToken;
					sexpr->sexpr_.push_back(myAtom);
				}
			}
			return i;
		}
};


//-----------------------------------------------------------------------------
//		FUNCTION DEFINITIONS FOR THE L-EXPRESSION STRUCT AND ITS PARSER
//-----------------------------------------------------------------------------

size_t LExprParser::parse_(size_t begin, LExpr* lexpr){
	size_t i;
	atom myAtom;
	pair<SExpr *,size_t> sres;
	SExprParser sParser;
	for(i=begin; i<tok_stream_.size(); i++){
		if(tok_stream_[i].type_ == OPERATOR){
			switch(tok_stream_[i].token_.op){
				case LPAREN:
					sParser.construct(tok_stream_, i+1);
					sres = sParser.getSExpr();
					i += sres.second;
					//atom myAtom;
					myAtom.type_ = SEXPR;
					myAtom.atom_ = (void *)sres.first;
					//lexpr->lexpr_.push_back(myAtom);
					break;

				case COMMA:
					comma_delimited_ = true;
					break;
			}
		}
	}
}


					

inline void LExpr::print(){
	/*cout<<"\n";
	indentLevel++;
	for(int i=0;i<indentLevel;i++) cout<<"\t";cout<<"{ ";
	for(size_t i=0; i<lexpr_.size(); i++){
		if(lexpr_[i].type_ == TOKEN){
			Token *t=(Token *) lexpr_[i].atom_;
			t->print();
			cout<<" ";
		}
		else if(lexpr_[i].type_ == SEXPR){
			indentLevel++;
			cout<<endl;
			for(int j=0; j<indentLevel; j++) cout<<"\t";cout<<"( ";
			SExpr *s=(SExpr *)lexpr_[i].atom_;
			s->print();
			indentLevel--;
			cout<<" )"<<endl;
			for(int j=0; j<indentLevel; j++) cout<<"\t";
		}
		else if(lexpr_[i].type_ == LEXPR){
			LExpr *l=(LExpr *)lexpr_[i].atom_;
			l->print();
		}
		else;

		if(!arithmatic_){
			cout<<" , ";
		}
	}
	indentLevel--;
	cout<<" } "<<endl;
	for(int j=0; j<indentLevel; j++) cout<<"\t";*/
}

pair<LExpr *,size_t> LExprParser::getLExpr(){}


//-----------------------------------------------------------------------------

struct Statement{
	bool store_;
	SExpr *lval_;
	SExpr *rval_;

	Statement():store_(true),lval_(NULL),rval_(NULL){}
	void print(){
		cout<<endl<<" L - V A L U E"<<endl<<"================"<<endl;
		lval_->print();
		if(store_){
			cout<<endl<<" R - V A L U E"<<endl<<"================"<<endl;
			rval_->print();
		}
	}

	~Statement(){
		delete lval_;
		delete rval_;
	}
};

class StatementParser{
	public:
		StatementParser(const char *expr):expr_(expr){
			Tokenizer lex_(expr);
			Token t;
			vector<Token> tok_stream_l;
			vector<Token> tok_stream_r;

			stmt_ = new Statement;

			while(lex_.next(t)){
				if(t.type_ == OPERATOR){
					if(t.token_.op == ASSIGN){
						break;
					}
					else tok_stream_l.push_back(t);
				}
				else tok_stream_l.push_back(t);
			}
			SExprParser lval_parser_(tok_stream_l);
			stmt_->lval_ = lval_parser_.getSExpr().first;
			int count = 0;
			while(lex_.next(t)){
				count++;
				tok_stream_r.push_back(t);
			}
			if(!count)stmt_->store_=false;
			else{
				SExprParser rval_parser_(tok_stream_r);
				stmt_->rval_ = rval_parser_.getSExpr().first;
			}
		}

		Statement *getStmt(){
			return stmt_;
		}


	private:
		const char *expr_;
		vector <Token> tok_stream_;
		Statement *stmt_;
};



class SymTbl{
	public:
		typedef SymTbl type;
		typedef string key_type;
		typedef bit64 value_type;
		typedef bit64 count_type;

		SymTbl():offset(0),sym_count(0){}
		bool insert(const key_type key){
			if(table.count(key)) return false;
			table[key]=++sym_count;
			return true;
		}

		value_type operator [](const key_type &key){
			return offset+table[key];
		}

		const bool exists(const key_type &key) const{
			if(table.count(key)) return true;
			return false;
		}

		const bool empty() const{
			return sym_count?false:true;
		}

	private:
		count_type offset;
		count_type sym_count;
		map<key_type, value_type> table;
};


class machine{
	public:
		typedef bit64 word_type;
		typedef bit8 byte_type;
//-----------------------------------------------------------------------------
//     TO WORK FROM HERE. IMPLEMENT THE VIRTUAL MACHINE FOR INTERPRETATION
//     ALSO IMPLEMENT AN INTERMEDIATE REPRESENTATION IN BINARY IF POSSIBLE 
//-----------------------------------------------------------------------------
		machine():current_state(0){}
		bool storeStatement(Statement *s){}
		bool eval(SExpr *expr){}
			

	private:
		word_type current_state;
		SymTbl sym_table;
		map<pair<word_type,word_type>,word_type> transition_table;
		set<word_type> final_states;
		vector<vector<SExpr *> > eval_rules;
		map<word_type, size_t> binder_table;
};



int main(){
	line_no = 1;
	col_no = 0;
	cout<<"This is a (testing sentence (a+b) > 7.5 ) = (5! is 120 ) and"
		<<" I am \"Ganesh Prasad 1993\""<<endl;
	StatementParser parse("This is a (testing sentence (a+b) > 7.5 )"
						" = (5! is 120) and I am \"Ganesh Prasad 1993\"");
	Statement *expr = parse.getStmt();
	expr->print();
	cout<<endl;
	return 0;
}

#endif

