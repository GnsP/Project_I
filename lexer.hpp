#ifndef I_LEXER_H_
#define I_LEXER_H_

#include "includes.hpp"
using namespace std;


enum TokenType{ SYMBOL = 1536, INTEGER, REAL, OPERATOR, STRING , _NONE, VAR_MARKER };
const char *typeString(TokenType t);

enum Operator{	PLUS = 7654, MINUS, MULT, DIV, MOD, POW,
				AND, OR, NOT, XOR,
				GT, LT, GTE, LTE, EQ, NEQ , 
				COND, ELSE, CONSTRAINT,
				LPAREN, RPAREN, ERROR, 
				LCURL, RCURL, LBRACKET, RBRACKET,
				ESCAPE, ASSIGN, COMMA, VAR };

const char *opString(Operator op);
Operator str2op(const char *str);

union TokenHolder{
	int integer;
	double real;
	char str[1024];
	char sym[1024];
	Operator op;
};

class Token{
	public:
		TokenType type_;
		TokenHolder token_;

		int node_id_;

		Token():type_(_NONE){
		}
		Token(TokenType type, const char *tok_str){
			makeToken(type, tok_str);
		}

		Token (const Token &oldToken){
			type_ = oldToken.type_;
			token_ = oldToken.token_;
		}

		void makeToken(TokenType type, const char *tok_str);

		inline TokenType getType(){ return type_; }
		inline void print(){
			cout<<"<"<<typeString(type_)<<">";
			switch(type_){
				case INTEGER:   cout<<token_.integer;           break;
				case REAL:      cout<<token_.real;              break;
				case SYMBOL:    cout<<token_.sym;               break;
				case STRING:    cout<<token_.str;               break;
				case VAR_MARKER:
				case OPERATOR:  cout<<opString(token_.op);      break;
				case _NONE:     cout<<" ";                           break;
				default:        cout<<" ";                           break;
			}
			cout<<"</"<<typeString(type_)<<">"<<endl;
		}

#ifdef PARSE_OUT_DOT_
		inline void printDot(int parent){
#ifdef PARSE_OUT_DOT_
			node_id_ = nodeCounter++;
			//cout<<node_id_<<endl;
#endif
			sstream nodeName("node_");
			sstream parName("node_");
			nodeName << node_id_;
			parName << parent;
			cout<<parName.str()<<"->"<<nodeName.str()<<";"<<endl;
			cout<<nodeName.str()
				<<"[shape=box, fontsize=14, fontname=\"Consolas\", label=\""
				<<typeString(type_)<<"\\n";
			switch(type_){
				case INTEGER:   cout<<token_.integer;           break;
				case REAL:      cout<<token_.real;              break;
				case SYMBOL:    cout<<token_.sym;               break;
				case STRING:    cout<<token_.str;               break;
				case VAR_MARKER:
				case OPERATOR:  cout<<opString(token_.op);      break;
				case _NONE:     cout<<" ";                           break;
				default:        cout<<" ";                           break;
			}
			cout<<"\"];"<<endl;
		}
#endif
};

class Tokenizer{
	public:
		Tokenizer():expr_(NULL),type_(_NONE){}
		Tokenizer(const char *expr):expr_(expr){}

		inline bool next(Token &t){
			bool ret = getNextToken();
			t.makeToken(type_, token_);
			return ret;
		}

		inline const char* getExpr(){ return expr_; }

	private:
		const char *expr_;
		char token_[1024];
		TokenType type_;

		inline bool isOperator(char ch){
			if(strchr("+-*/%^=(){}[]&|~><?:@,\\", ch)) return true;
			return false;
		}

		bool getNextToken();
};

#endif
