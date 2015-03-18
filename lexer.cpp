#include "lexer.hpp"
using namespace std;

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
		case VAR_MARKER:
			return "VAR_MARKER";
		default:
			return "ERROR ...";
	}
}

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
		case VAR:	return "VAR";
		case LBRACKET:	return "LBRACKET";
		case RBRACKET:	return "RBRACKET";
		case CONSTRAINT:return "CONSTRAINT";
		case COND:	return "COND";
		case ELSE:	return "ELSE";
		default:    return "\0";
	}
}

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
		case '$': return VAR;
		case '[': return LBRACKET;
		case ']': return RBRACKET;
		case '?': return COND;
		case ':': return ELSE;
		case '@': return CONSTRAINT;
		default:
			return ERROR;
	}
}


void Token::makeToken(TokenType type, const char *tok_str){
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
		case VAR_MARKER:
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


bool Tokenizer::getNextToken(){
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
		switch(*expr_){
			case '*':
				*temp++ = *expr_++;
				col_no++;
				if(*(expr_)=='*'){
					*temp++ = *expr_++;
					col_no++;
				}
				break;
			case '~':
			case '>':
			case '<':
			case '=':
				*temp++ = *expr_++;
				col_no++;
				if(*(expr_)=='='){ 
					*temp++ = *expr_++;
					col_no++;
				}
				break;
			default:
				*temp++ = *expr_++;
				col_no++;
				break;
		}
		type_ = OPERATOR;
	}
	else if(*expr_ == '$'){
		*temp++ = *expr_++;
		col_no++;
		type_ = VAR_MARKER;
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
