#ifndef I_PARSER_H_
#define I_PARSER_H_

#include "lexer.hpp"
using namespace std;

struct SExpr;
struct Indexed;

struct Indexed{
	SExpr *index_;
	int node_id_;
	void operator=(const Indexed ind){
		index_=ind.index_;
	}
#ifdef PARSE_OUT_DOT_
	int mode_id_;
	void printDot(int parent);

	Indexed():node_id_(nodeCounter++){}
#endif
	void print();
};

enum atomType{ TOKEN, SEXPR, LEXPR, VARIABLE };
struct atom{
	bool indexed_;
	Indexed index_;
	atomType type_;
	void *atom_;
#ifdef PARSE_OUT_DOT_
	int node_id_;
	void printDot(int parent);

	atom():node_id_(nodeCounter++){};
#endif

	void print();
};

struct Logical_not{
	bool not_;
	vector<atom> logical_not_;		// Unary operator ~
#ifdef PARSE_OUT_DOT_
	int node_id_;
	void printDot(int parent);

#endif

	Logical_not(){
		not_ = false;
#ifdef PARSE_OUT_DOT_
		node_id_ = nodeCounter++;
#endif
	}
	void print();
};

struct Logical_xor{
	vector<Logical_not> logical_xor_;	// values separated by ^
#ifdef PARSE_OUT_DOT_
	int node_id_;
	void printDot(int parent);

	Logical_xor():node_id_(nodeCounter++){};
#endif
	void print();
};

struct Logical_and{
	vector<Logical_xor> logical_and_;	// values separated by &
#ifdef PARSE_OUT_DOT_
	int node_id_;
	void printDot(int parent);
	
	Logical_and():node_id_(nodeCounter++){};
#endif
	void print();
};

struct Logical_or{
	vector<Logical_and> logical_or_; // Values separated by |
#ifdef PARSE_OUT_DOT_
	int node_id_;
	void printDot(int parent);

	Logical_or():node_id_(nodeCounter++){};
#endif
	void print();
};

struct Unary{
	bool minus_;
	vector<Logical_or> unary_;	// Unary operators +, -
#ifdef PARSE_OUT_DOT_
	int node_id_;
	void printDot(int parent);
#endif

	Unary(){
		minus_ = false;
#ifdef PARSE_OU_DOT_
		node_id_ = nodeCounter++;
#endif
	}
	void print();
};

struct Exponent{
	vector<Unary> exponent_;	// Values separated by exponent op **
#ifdef PARSE_OUT_DOT_
	int node_id_;
	void printDot(int parent);

	Exponent():node_id_(nodeCounter++){};
#endif
	void print();
};

struct Modulus{					// Values separated by %
	vector<Exponent> modulus_;
#ifdef PARSE_OUT_DOT_
	int node_id_;
	void printDot(int parent);

	Modulus():node_id_(nodeCounter++){};
#endif
	void print();
};

struct FactorDiv{					// Values separated by *, /
	vector<Modulus> factor_div_;
#ifdef PARSE_OUT_DOT_
	int node_id_;
	void printDot(int parent);

	FactorDiv():node_id_(nodeCounter++){};
#endif
	void print();
};

struct FactorMult{
	vector<FactorDiv> factor_mult_;
#ifdef PARSE_OUT_DOT_
	int node_id_;
	void printDot(int parent);

	FactorMult():node_id_(nodeCounter++){};
#endif
	void print();
};

struct TermAdd{					//Values separated by + and -
	vector<FactorMult> term_add_;
#ifdef PARSE_OUT_DOT_
	int node_id_;
	void printDot(int parent);

	TermAdd():node_id_(nodeCounter++){};
#endif
	void print();
};

struct TermSub{
	vector<TermAdd> term_sub_;
#ifdef PARSE_OUT_DOT_
	int node_id_;
	void printDot(int parent);

	TermSub():node_id_(nodeCounter++){};
#endif
	void print();
};

struct Compare_Gt{
	vector<TermSub> compare_gt_;
#ifdef PARSE_OUT_DOT_
	int node_id_;
	void printDot(int parent);

	Compare_Gt():node_id_(nodeCounter++){};
#endif
	void print();
};

struct Compare_Lt{
	vector<Compare_Gt> compare_lt_;
#ifdef PARSE_OUT_DOT_
	int node_id_;
	void printDot(int parent);

	Compare_Lt():node_id_(nodeCounter++){};
#endif
	void print();
};

struct Compare_Gte{
	vector<Compare_Lt> compare_gte_;
#ifdef PARSE_OUT_DOT_
	int node_id_;
	void printDot(int parent);

	Compare_Gte():node_id_(nodeCounter++){};
#endif
	void print();
};

struct Compare_Lte{
	vector<Compare_Gte> compare_lte_;
#ifdef PARSE_OUT_DOT_
	int node_id_;
	void printDot(int parent);

	Compare_Lte():node_id_(nodeCounter++){};
#endif
	void print();
};

struct Compare_Neq{
	vector<Compare_Lte> compare_neq_;
#ifdef PARSE_OUT_DOT_
	int node_id_;
	void printDot(int parent);

	Compare_Neq():node_id_(nodeCounter++){};
#endif
	void print();
};

struct Compare_Eq{
	vector<Compare_Neq> compare_eq_;
#ifdef PARSE_OUT_DOT_
	int node_id_;
	void printDot(int parent);

	Compare_Eq():node_id_(nodeCounter++){};
#endif
	void print();
};

struct Component{				//The if-else operator _?_:_
	vector<Compare_Eq> component_;
#ifdef PARSE_OUT_DOT_
	int node_id_;
	void printDot(int parent);

	Component():node_id_(nodeCounter++){};
#endif
	void print();
};

struct LExpr{					//Comma separated values
	vector<Component> lexpr_;
#ifdef PARSE_OUT_DOT_
	int node_id_;
	void printDot(int parent);

	LExpr():node_id_(nodeCounter++){};
#endif
	void print();
};

struct Variable{
	LExpr *context_;
	char var_name_[1024];

#ifdef PARSE_OUT_DOT_
	int node_id_;
	void printDot(int parent);
#endif
	Variable(){
		context_ = NULL;
#ifdef PARSE_OUT_DOT_
		node_id_ = nodeCounter++;
#endif
	}

	void print();
};


struct SExpr{
	vector<atom> sexpr_;
#ifdef PARSE_OUT_DOT_
	int node_id_;
	void printDot(int parent);

	SExpr():node_id_(nodeCounter++){};
#endif
	void print();
};

struct Statement{
	bool store_;
	SExpr *lval_;
	SExpr *rval_;

#ifdef PARSE_OUT_DOT_
	int node_id_;
	void printDot(int parent);
#endif
	Statement():store_(true),lval_(NULL),rval_(NULL){
#ifdef PARSE_OUT_DOT_
		node_id_ = nodeCounter++;
#endif
	}

	~Statement(){
		delete lval_;
		delete rval_;
	}

	inline void print(){
		cout<<"<STATEMENT store_=\""<<(store_?"true":"false")<<"\">"<<endl;
		if(lval_ != NULL){
			cout<<"<L-VALUE>"<<endl;
			lval_->print();
			cout<<"</L-VALUE>"<<endl;
		}
		
		if(rval_ != NULL){
			cout<<"<R-VALUE>"<<endl;
			rval_->print();
			cout<<"</R-VALUE>"<<endl;
		}
		cout<<"</STATEMENT>"<<endl;
	}
};






class IndexedParser{
	private: size_t parse_(size_t begin, Indexed &expr);

	public:
		IndexedParser(const vector<Token> &tok_stream, size_t begin){
			for(size_t i=begin; i<tok_stream.size(); i++){
				tok_stream_.push_back(tok_stream[i]);
			}
		}
		IndexedParser(const vector<Token> &tok_stream)
			:tok_stream_(tok_stream){}
		IndexedParser(){}

		void construct(const vector<Token> &tok_stream, size_t begin){
			for(size_t i=begin; i<tok_stream.size(); i++){
				tok_stream_.push_back(tok_stream[i]);
			}
		}

		void destruct(){
			vector<Token>().swap(tok_stream_);		//clear and dellocate memory
		}

		inline void setDelimiter(Operator op){
			delimiter_ = op;
		}
		
		pair<Indexed, size_t> getIndexed();

	private:
		vector<Token> tok_stream_;
		Operator delimiter_;		
		
};


class Logical_notParser{
	private: size_t parse_(size_t begin, Logical_not &expr);

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

		void destruct(){
			vector<Token>().swap(tok_stream_);		//clear and dellocate memory
		}

		inline void setDelimiter(Operator op){
			delimiter_ = op;
		}
		
		pair<Logical_not, size_t> getLogical_not();

	private:
		vector<Token> tok_stream_;
		Operator delimiter_;		
		
};

class Logical_xorParser{
	public:
		Logical_xorParser(const vector<Token> &tok_stream, size_t begin){
			for(size_t i=begin; i<tok_stream.size(); i++){
				tok_stream_.push_back(tok_stream[i]);
			}
		}
		Logical_xorParser(const vector<Token> &tok_stream)
			:tok_stream_(tok_stream){}
		Logical_xorParser(){}

		void construct(const vector<Token> &tok_stream, size_t begin){
			for(size_t i=begin; i<tok_stream.size(); i++){
				tok_stream_.push_back(tok_stream[i]);
			}
		}
		void destruct(){
			vector<Token>().swap(tok_stream_);		//clear and dellocate memory
		}
		inline void setDelimiter(Operator op){
			delimiter_ = op;
		}
		pair<Logical_xor, size_t> getLogical_xor();

	private:
		vector<Token> tok_stream_;
		Operator delimiter_;
		
		size_t parse_(size_t begin, Logical_xor &expr);

};

class Logical_andParser{
	public:
		Logical_andParser(const vector<Token> &tok_stream, size_t begin){
			for(size_t i=begin; i<tok_stream.size(); i++){
				tok_stream_.push_back(tok_stream[i]);
			}
		}
		Logical_andParser(const vector<Token> &tok_stream)
			:tok_stream_(tok_stream){}
		Logical_andParser(){}

		void construct(const vector<Token> &tok_stream, size_t begin){
			for(size_t i=begin; i<tok_stream.size(); i++){
				tok_stream_.push_back(tok_stream[i]);
			}
		}
		void destruct(){
			vector<Token>().swap(tok_stream_);		//clear and dellocate memory
		}
		
		inline void setDelimiter(Operator op){
			delimiter_ = op;
		}
		
		pair<Logical_and, size_t> getLogical_and();

	private:
		vector<Token> tok_stream_;
		Operator delimiter_;
		
		size_t parse_(size_t begin, Logical_and &expr);

};

class Logical_orParser{
	public:
		Logical_orParser(const vector<Token> &tok_stream, size_t begin){
			for(size_t i=begin; i<tok_stream.size(); i++){
				tok_stream_.push_back(tok_stream[i]);
			}
		}
		Logical_orParser(const vector<Token> &tok_stream)
			:tok_stream_(tok_stream){}
		Logical_orParser(){}

		void construct(const vector<Token> &tok_stream, size_t begin){
			for(size_t i=begin; i<tok_stream.size(); i++){
				tok_stream_.push_back(tok_stream[i]);
			}
		}
		void destruct(){
			vector<Token>().swap(tok_stream_);		//clear and dellocate memory
		}

		inline void setDelimiter(Operator op){
			delimiter_ = op;
		}

		pair<Logical_or, size_t> getLogical_or();

	private:
		vector<Token> tok_stream_;
		Operator delimiter_;

		size_t parse_(size_t begin, Logical_or &expr);

};

class UnaryParser{
	public:
		UnaryParser(const vector<Token> &tok_stream, size_t begin){
			for(size_t i=begin; i<tok_stream.size(); i++){
				tok_stream_.push_back(tok_stream[i]);
			}
		}
		UnaryParser(const vector<Token> &tok_stream)
			:tok_stream_(tok_stream){}
		UnaryParser(){}

		void construct(const vector<Token> &tok_stream, size_t begin){
			for(size_t i=begin; i<tok_stream.size(); i++){
				tok_stream_.push_back(tok_stream[i]);
			}
		}

		void destruct(){
			vector<Token>().swap(tok_stream_);		//clear and dellocate memory
		}
		inline void setDelimiter(Operator op){
			delimiter_ = op;
		}

		pair<Unary, size_t> getUnary();

	private:
		vector<Token> tok_stream_;
		Operator delimiter_;

		size_t parse_(size_t begin, Unary &expr);

};

class ExponentParser{
	public:
		ExponentParser(const vector<Token> &tok_stream, size_t begin){
			for(size_t i=begin; i<tok_stream.size(); i++){
				tok_stream_.push_back(tok_stream[i]);
			}
		}
		ExponentParser(const vector<Token> &tok_stream)
			:tok_stream_(tok_stream){}
		ExponentParser(){}

		void construct(const vector<Token> &tok_stream, size_t begin){
			for(size_t i=begin; i<tok_stream.size(); i++){
				tok_stream_.push_back(tok_stream[i]);
			}
		}
		void destruct(){
			vector<Token>().swap(tok_stream_);		//clear and dellocate memory
		}

		inline void setDelimiter(Operator op){
			delimiter_ = op;
		}

		pair<Exponent, size_t> getExponent();

	private:
		vector<Token> tok_stream_;
		Operator delimiter_;

		size_t parse_(size_t begin, Exponent &expr);

};

class ModulusParser{
	public:
		ModulusParser(const vector<Token> &tok_stream, size_t begin){
			for(size_t i=begin; i<tok_stream.size(); i++){
				tok_stream_.push_back(tok_stream[i]);
			}
		}
		ModulusParser(const vector<Token> &tok_stream)
			:tok_stream_(tok_stream){}
		ModulusParser(){}

		void construct(const vector<Token> &tok_stream, size_t begin){
			for(size_t i=begin; i<tok_stream.size(); i++){
				tok_stream_.push_back(tok_stream[i]);
			}
		}
		void destruct(){
			vector<Token>().swap(tok_stream_);		//clear and dellocate memory
		}

		inline void setDelimiter(Operator op){
			delimiter_ = op;
		}

		pair<Modulus, size_t> getModulus();

	private:
		vector<Token> tok_stream_;
		Operator delimiter_;

		size_t parse_(size_t begin, Modulus &expr);

};

class FactorDivParser{
	public:
		FactorDivParser(const vector<Token> &tok_stream, size_t begin){
			for(size_t i=begin; i<tok_stream.size(); i++){
				tok_stream_.push_back(tok_stream[i]);
			}
		}
		FactorDivParser(const vector<Token> &tok_stream)
			:tok_stream_(tok_stream){}
		FactorDivParser(){}

		void construct(const vector<Token> &tok_stream, size_t begin){
			for(size_t i=begin; i<tok_stream.size(); i++){
				tok_stream_.push_back(tok_stream[i]);
			}
		}
		void destruct(){
			vector<Token>().swap(tok_stream_);		//clear and dellocate memory
		}

		inline void setDelimiter(Operator op){
			delimiter_ = op;
		}

		pair<FactorDiv, size_t> getFactorDiv();

	private:
		vector<Token> tok_stream_;
		Operator delimiter_;

		size_t parse_(size_t begin, FactorDiv &expr);

};

class FactorMultParser{
	public:
		FactorMultParser(const vector<Token> &tok_stream, size_t begin){
			for(size_t i=begin; i<tok_stream.size(); i++){
				tok_stream_.push_back(tok_stream[i]);
			}
		}
		FactorMultParser(const vector<Token> &tok_stream)
			:tok_stream_(tok_stream){}
		FactorMultParser(){}

		void construct(const vector<Token> &tok_stream, size_t begin){
			for(size_t i=begin; i<tok_stream.size(); i++){
				tok_stream_.push_back(tok_stream[i]);
			}
		}
		void destruct(){
			vector<Token>().swap(tok_stream_);		//clear and dellocate memory
		}

		inline void setDelimiter(Operator op){
			delimiter_ = op;
		}

		pair<FactorMult, size_t> getFactorMult();

	private:
		vector<Token> tok_stream_;
		Operator delimiter_;

		size_t parse_(size_t begin, FactorMult &expr);

};

class TermAddParser{
	public:
		TermAddParser(const vector<Token> &tok_stream, size_t begin){
			for(size_t i=begin; i<tok_stream.size(); i++){
				tok_stream_.push_back(tok_stream[i]);
			}
		}
		TermAddParser(const vector<Token> &tok_stream)
			:tok_stream_(tok_stream){}
		TermAddParser(){}

		void construct(const vector<Token> &tok_stream, size_t begin){
			for(size_t i=begin; i<tok_stream.size(); i++){
				tok_stream_.push_back(tok_stream[i]);
			}
		}
		void destruct(){
			vector<Token>().swap(tok_stream_);		//clear and dellocate memory
		}

		inline void setDelimiter(Operator op){
			delimiter_ = op;
		}

		pair<TermAdd, size_t> getTermAdd();

	private:
		vector<Token> tok_stream_;
		Operator delimiter_;

		size_t parse_(size_t begin, TermAdd &expr);

};

class TermSubParser{
	public:
		TermSubParser(const vector<Token> &tok_stream, size_t begin){
			for(size_t i=begin; i<tok_stream.size(); i++){
				tok_stream_.push_back(tok_stream[i]);
			}
		}
		TermSubParser(const vector<Token> &tok_stream)
			:tok_stream_(tok_stream){}
		TermSubParser(){}

		void construct(const vector<Token> &tok_stream, size_t begin){
			for(size_t i=begin; i<tok_stream.size(); i++){
				tok_stream_.push_back(tok_stream[i]);
			}
		}
		void destruct(){
			vector<Token>().swap(tok_stream_);		//clear and dellocate memory
		}

		inline void setDelimiter(Operator op){
			delimiter_ = op;
		}

		pair<TermSub, size_t> getTermSub();

	private:
		vector<Token> tok_stream_;
		Operator delimiter_;

		size_t parse_(size_t begin, TermSub &expr);

};

class Compare_GtParser{
	public:
		Compare_GtParser(const vector<Token> &tok_stream, size_t begin){
			for(size_t i=begin; i<tok_stream.size(); i++){
				tok_stream_.push_back(tok_stream[i]);
			}
		}
		Compare_GtParser(const vector<Token> &tok_stream)
			:tok_stream_(tok_stream){}
		Compare_GtParser(){}

		void construct(const vector<Token> &tok_stream, size_t begin){
			for(size_t i=begin; i<tok_stream.size(); i++){
				tok_stream_.push_back(tok_stream[i]);
			}
		}
		void destruct(){
			vector<Token>().swap(tok_stream_);		//clear and dellocate memory
		}

		inline void setDelimiter(Operator op){
			delimiter_ = op;
		}

		pair<Compare_Gt, size_t> getCompare_Gt();

	private:
		vector<Token> tok_stream_;
		Operator delimiter_;

		size_t parse_(size_t begin, Compare_Gt &expr);

};

class Compare_LtParser{
	public:
		Compare_LtParser(const vector<Token> &tok_stream, size_t begin){
			for(size_t i=begin; i<tok_stream.size(); i++){
				tok_stream_.push_back(tok_stream[i]);
			}
		}
		Compare_LtParser(const vector<Token> &tok_stream)
			:tok_stream_(tok_stream){}
		Compare_LtParser(){}

		void construct(const vector<Token> &tok_stream, size_t begin){
			for(size_t i=begin; i<tok_stream.size(); i++){
				tok_stream_.push_back(tok_stream[i]);
			}
		}
		void destruct(){
			vector<Token>().swap(tok_stream_);		//clear and dellocate memory
		}

		inline void setDelimiter(Operator op){
			delimiter_ = op;
		}

		pair<Compare_Lt, size_t> getCompare_Lt();

	private:
		vector<Token> tok_stream_;
		Operator delimiter_;

		size_t parse_(size_t begin, Compare_Lt &expr);

};

class Compare_GteParser{
	public:
		Compare_GteParser(const vector<Token> &tok_stream, size_t begin){
			for(size_t i=begin; i<tok_stream.size(); i++){
				tok_stream_.push_back(tok_stream[i]);
			}
		}
		Compare_GteParser(const vector<Token> &tok_stream)
			:tok_stream_(tok_stream){}
		Compare_GteParser(){}

		void construct(const vector<Token> &tok_stream, size_t begin){
			for(size_t i=begin; i<tok_stream.size(); i++){
				tok_stream_.push_back(tok_stream[i]);
			}
		}
		void destruct(){
			vector<Token>().swap(tok_stream_);		//clear and dellocate memory
		}

		inline void setDelimiter(Operator op){
			delimiter_ = op;
		}

		pair<Compare_Gte, size_t> getCompare_Gte();

	private:
		vector<Token> tok_stream_;
		Operator delimiter_;

		size_t parse_(size_t begin, Compare_Gte &expr);

};

class Compare_LteParser{
	public:
		Compare_LteParser(const vector<Token> &tok_stream, size_t begin){
			for(size_t i=begin; i<tok_stream.size(); i++){
				tok_stream_.push_back(tok_stream[i]);
			}
		}
		Compare_LteParser(const vector<Token> &tok_stream)
			:tok_stream_(tok_stream){}
		Compare_LteParser(){}

		void construct(const vector<Token> &tok_stream, size_t begin){
			for(size_t i=begin; i<tok_stream.size(); i++){
				tok_stream_.push_back(tok_stream[i]);
			}
		}
		void destruct(){
			vector<Token>().swap(tok_stream_);		//clear and dellocate memory
		}

		inline void setDelimiter(Operator op){
			delimiter_ = op;
		}

		pair<Compare_Lte, size_t> getCompare_Lte();

	private:
		vector<Token> tok_stream_;
		Operator delimiter_;

		size_t parse_(size_t begin, Compare_Lte &expr);

};

class Compare_NeqParser{
	public:
		Compare_NeqParser(const vector<Token> &tok_stream, size_t begin){
			for(size_t i=begin; i<tok_stream.size(); i++){
				tok_stream_.push_back(tok_stream[i]);
			}
		}
		Compare_NeqParser(const vector<Token> &tok_stream)
			:tok_stream_(tok_stream){}
		Compare_NeqParser(){}

		void construct(const vector<Token> &tok_stream, size_t begin){
			for(size_t i=begin; i<tok_stream.size(); i++){
				tok_stream_.push_back(tok_stream[i]);
			}
		}
		void destruct(){
			vector<Token>().swap(tok_stream_);		//clear and dellocate memory
		}

		inline void setDelimiter(Operator op){
			delimiter_ = op;
		}

		pair<Compare_Neq, size_t> getCompare_Neq();

	private:
		vector<Token> tok_stream_;
		Operator delimiter_;

		size_t parse_(size_t begin, Compare_Neq &expr);

};

class Compare_EqParser{
	public:
		Compare_EqParser(const vector<Token> &tok_stream, size_t begin){
			for(size_t i=begin; i<tok_stream.size(); i++){
				tok_stream_.push_back(tok_stream[i]);
			}
		}
		Compare_EqParser(const vector<Token> &tok_stream)
			:tok_stream_(tok_stream){}
		Compare_EqParser(){}

		void construct(const vector<Token> &tok_stream, size_t begin){
			for(size_t i=begin; i<tok_stream.size(); i++){
				tok_stream_.push_back(tok_stream[i]);
			}
		}
		void destruct(){
			vector<Token>().swap(tok_stream_);		//clear and dellocate memory
		}

		inline void setDelimiter(Operator op){
			delimiter_ = op;
		}

		pair<Compare_Eq, size_t> getCompare_Eq();

	private:
		vector<Token> tok_stream_;
		Operator delimiter_;

		size_t parse_(size_t begin, Compare_Eq &expr);

};

class ComponentParser{
	public:
		ComponentParser(const vector<Token> &tok_stream, size_t begin){
			for(size_t i=begin; i<tok_stream.size(); i++){
				tok_stream_.push_back(tok_stream[i]);
			}
		}
		ComponentParser(const vector<Token> &tok_stream)
			:tok_stream_(tok_stream){}
		ComponentParser(){}

		void construct(const vector<Token> &tok_stream, size_t begin){
			for(size_t i=begin; i<tok_stream.size(); i++){
				tok_stream_.push_back(tok_stream[i]);
			}
		}
		void destruct(){
			vector<Token>().swap(tok_stream_);		//clear and dellocate memory
		}

		inline void setDelimiter(Operator op){
			delimiter_ = op;
		}

		pair<Component, size_t> getComponent();

	private:
		vector<Token> tok_stream_;
		Operator delimiter_;

		size_t parse_(size_t begin, Component &expr);

};

class LExprParser{
	public:
		LExprParser(const vector<Token> &tok_stream, size_t begin){
			for(size_t i=begin; i<tok_stream.size(); i++){
				tok_stream_.push_back(tok_stream[i]);
			}
		}
		LExprParser(const vector<Token> &tok_stream):tok_stream_(tok_stream){}
		LExprParser(){
			delimiter_ = RCURL;
		}

		void construct(const vector<Token> &tok_stream, size_t begin){
			for(size_t i=begin; i<tok_stream.size(); i++){
				tok_stream_.push_back(tok_stream[i]);
			}
		}
		void destruct(){
			vector<Token>().swap(tok_stream_);		//clear and dellocate memory
		}

		inline void setDelimiter(Operator op){
			delimiter_ = op;
		}

		pair<LExpr *,size_t> getLExpr();
		
	private:
		const char *expr_;
		vector<Token> tok_stream_;
		Token tok_;
		LExpr *lexpr_;
		bit32 curl_match_;
		Operator delimiter_;

		size_t parse_(size_t begin, LExpr *lexpr);
};

class VariableParser{
	public:
		VariableParser(const vector<Token> &tok_stream, size_t begin){
			for(size_t i=begin; i<tok_stream.size(); i++){
				tok_stream_.push_back(tok_stream[i]);
			}
		}
		VariableParser(const vector<Token> &tok_stream):tok_stream_(tok_stream){}
		VariableParser(){}

		void construct(const vector<Token> &tok_stream, size_t begin){
			for(size_t i=begin; i<tok_stream.size(); i++){
				tok_stream_.push_back(tok_stream[i]);
			}
		}
		void destruct(){
			vector<Token>().swap(tok_stream_);		//clear and dellocate memory
		}

		inline void setDelimiter(Operator op){
			delimiter_ = op;
		}

		pair<Variable,size_t> getVariable();


	private:
		vector<Token> tok_stream_;
		Token tok_;
		Operator delimiter_;

		size_t parse_(size_t begin, Variable &var);
};


class SExprParser{
	public:
		SExprParser(const char *expr):expr_(expr),lex_(expr){
			while(lex_.next(tok_)) tok_stream_.push_back(tok_);
			bareDelimited_ = false;
		}
		SExprParser(const vector<Token> &tok_stream, size_t begin){
			for(size_t i=begin; i<tok_stream.size(); i++){
				tok_stream_.push_back(tok_stream[i]);
			}
			bareDelimited_ = false;
		}
		SExprParser(const vector<Token> &tok_stream):tok_stream_(tok_stream){
			bareDelimited_ = false;
		}
		SExprParser(){
			bareDelimited_ = false;
		}

		void construct(const vector<Token> &tok_stream, size_t begin){
			for(size_t i=begin; i<tok_stream.size(); i++){
				tok_stream_.push_back(tok_stream[i]);
			}
			delimiter_ = RPAREN;
		}
		void destruct(){
			vector<Token>().swap(tok_stream_);		//clear and dellocate memory
		}

		inline void setDelimiter(Operator op){
			delimiter_ = op;
		}

		inline void setBareDelimited(){
			bareDelimited_ = true;
		}
		
		pair<SExpr *,size_t> getSExpr();
	
	private:
		const char *expr_;
		vector<Token> tok_stream_;
		Token tok_;
		SExpr *sexpr_;
		Tokenizer lex_;
		bit32 paren_match;
		Operator delimiter_;
		bool bareDelimited_; 

		size_t parse_(size_t begin, SExpr *sexpr);
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

		inline Statement *getStmt(){
			return stmt_;
		}


	private:
		const char *expr_;
		vector <Token> tok_stream_;
		Statement *stmt_;
};

#endif
