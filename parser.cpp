#include "parser.hpp"
using namespace std;
size_t SExprParser::parse_(size_t begin, SExpr *sexpr){
	size_t i;
	atom myAtom;
	SExpr *newSexpr;
	LExprParser lParser;
	VariableParser vParser;
	IndexedParser iParser;
	pair<Variable, size_t> vres;
	Variable *var;
	SExprParser sParser;
	pair<SExpr *, size_t> sres;
	pair<LExpr *,size_t> lres;
	pair<Indexed, size_t> ires;


	for(i=begin; i<tok_stream_.size(); i++){
		//tok_stream_[i].print();
		if(tok_stream_[i].type_ == OPERATOR){
			switch(tok_stream_[i].token_.op){
				case LPAREN:	
					paren_match++;
					sParser.construct(tok_stream_, ++i);
					sParser.setDelimiter(RPAREN);
					sres = sParser.getSExpr();
					i += sres.second;
					newSexpr = sres.first;
					myAtom.type_=SEXPR;
					myAtom.atom_ = (void *)newSexpr;
					sexpr->sexpr_.push_back(myAtom);
					break;
				case LCURL:
					lParser.construct(tok_stream_, ++i);
					lParser.setDelimiter(RCURL);
					lres = lParser.getLExpr();
					i += lres.second;
					myAtom.type_ = LEXPR;
					myAtom.atom_ = (void *)lres.first;
					sexpr->sexpr_.push_back(myAtom);
					break;
				case LBRACKET:
					iParser.construct(tok_stream_, ++i);
					iParser.setDelimiter(RBRACKET);
					ires = iParser.getIndexed();
					i += ires.second;
					sexpr->sexpr_[sexpr->sexpr_.size()-1].indexed_=true;
					sexpr->sexpr_[sexpr->sexpr_.size()-1].index_=ires.first;
/*
					cout<<"\n\n\n\n";
					sexpr->print();
					cout<<"\n\n\n\n";
*/
					break;
				default:
					if(bareDelimited_){
						return i;
					}
					if(tok_stream_[i].token_.op == delimiter_){
						return ++i;
					}
					myAtom.type_=TOKEN;
					Token *myToken = new Token(tok_stream_[i]);
					myAtom.atom_ = (void *)myToken;
					sexpr->sexpr_.push_back(myAtom);
					break;
			}
		}
		else if(tok_stream_[i].type_ == VAR_MARKER){
			vParser.construct(tok_stream_, ++i);
			vres = vParser.getVariable();
			i += vres.second;
			var = new Variable(vres.first);
			myAtom.type_ = VARIABLE;
			myAtom.atom_ = (void *)var;
			sexpr->sexpr_.push_back(myAtom);
		}
		else{
			myAtom.type_ = TOKEN;
			Token *myToken = new Token(tok_stream_[i]);
			myAtom.atom_ = (void *)myToken;
			sexpr->sexpr_.push_back(myAtom);
		}
	}
	//cout<<"\n\n";
	return i;
}


pair<SExpr *,size_t> SExprParser::getSExpr(){
	pair<SExpr *, size_t> ret;
	paren_match = 0;
	ret.first = new SExpr; 
	ret.second = parse_(0,ret.first);
	destruct();
	/*if(paren_match){
		I_error e("Parentheses mismatch");
		e.print();
	}*/
	return ret;
}

pair<Indexed,size_t> IndexedParser::getIndexed(){
	pair<Indexed, size_t> ret;
	Indexed ind;
	ret.first = ind;
	ret.second = parse_(0,ret.first);
	destruct();
	return ret;
}

size_t IndexedParser::parse_(size_t begin, Indexed &ind){
	size_t i;
	i = begin;
	SExprParser sParser;
	pair<SExpr *,size_t> sres;

	if(tok_stream_[i].type_==OPERATOR){
		if(tok_stream_[i].token_.op == RBRACKET){
			ind.index_=NULL;
			return ++i;
		}
	}
	sParser.construct(tok_stream_,i); // was ++i before change
	sParser.setDelimiter(RBRACKET);
	sres = sParser.getSExpr();
	i += sres.second;
	ind.index_=sres.first;
	return i;
}

pair<Variable, size_t> VariableParser::getVariable(){
	pair<Variable, size_t> ret;
	Variable var;
	ret.first = var;
	ret.second = parse_(0, ret.first);
	destruct();
	return ret;
}

size_t VariableParser::parse_(size_t begin, Variable &var){
	size_t i;
	LExprParser lParser;
	pair<LExpr *,size_t> lres;

	i=begin;
	if(tok_stream_[i].type_ == OPERATOR){
		if(tok_stream_[i].token_.op==LBRACKET){
			lParser.construct(tok_stream_,++i);
			lParser.setDelimiter(RBRACKET);
			lres = lParser.getLExpr();
			i += lres.second;

			var.context_ = lres.first;
			i++;
		}
		else{
			I_error e("1 Wrong usage of VARIABLE");
			e.print();
		}
	}
	else if(tok_stream_[i].type_==SYMBOL);
	else{
		I_error e("2 Wrong usage of VARIABLE");
		e.print();
	}
//	tok_stream_[i].print();
	if(tok_stream_[i].type_ == SYMBOL){
		strcpy(var.var_name_, tok_stream_[i].token_.sym);
		return ++i;
	}
	else{
		I_error e("3 Wrong usage of VARIABLE");
		e.print();
	}
}




pair<Logical_not, size_t> Logical_notParser::getLogical_not(){
	pair<Logical_not, size_t> ret;
	Logical_not expr;
	ret.first = expr;
	ret.second = parse_(0, ret.first);
	destruct();
	return ret;
}

size_t Logical_notParser::parse_(size_t begin, Logical_not &expr){
	size_t i = begin;
	SExprParser sParser;
	VariableParser vParser;
	LExprParser lParser;
	IndexedParser iParser;
	Variable *var;
	pair<Variable, size_t> vres;
	pair<SExpr *, size_t> sres;
	pair<LExpr *, size_t> lres;
	pair<Indexed, size_t> ires;
	atom myAtom;

	if(tok_stream_[begin].type_==OPERATOR){
		if(tok_stream_[begin].token_.op==NOT){
			expr.not_ = true;
			i++;
		}
		else expr.not_ = false;
	}

	size_t count = 0,j;
	for(j=i; j<tok_stream_.size(); j++){
		//tok_stream_[j].print();
		if(tok_stream_[j].type_ == VAR_MARKER){
			vParser.construct(tok_stream_, ++j);
			vres = vParser.getVariable();
			j += vres.second;
		}

		if(tok_stream_[j].type_==OPERATOR){
			if(tok_stream_[j].token_.op == LPAREN){
				sParser.construct(tok_stream_, ++j);
				sres = sParser.getSExpr();
				j += sres.second;
				j--;
			}
			else if(tok_stream_[j].token_.op == LCURL){
				lParser.construct(tok_stream_, ++j);
				lres = lParser.getLExpr();
				j += lres.second;
				j--;
			}

			else if(tok_stream_[j].token_.op == LBRACKET){
				iParser.construct(tok_stream_, ++j);
				ires = iParser.getIndexed();
				j += ires.second;
				j--;
				//count--;
			}


			else if(tok_stream_[j].token_.op == delimiter_){
				//count--;
				break;
			}
			else{
				break;
			}
		}
		count++;
	}
	//cout<<"\n\n";
	if(count > 1){
		sParser.construct(tok_stream_, i);
		sParser.setBareDelimited();
		//cout<<endl<<"BARE DELIMITED SET\n"<<endl;
		sres = sParser.getSExpr();
		i += sres.second;
		myAtom.type_ = SEXPR;
		myAtom.atom_ = (void *)sres.first;
		expr.logical_not_.push_back(myAtom);
	}
	else{
		//tok_stream_[i].print();
		if(tok_stream_[i].type_ == VAR_MARKER){
			myAtom.type_ = VARIABLE;
			var = new Variable(vres.first);
			myAtom.atom_ = (void *)var;
			expr.logical_not_.push_back(myAtom);
		}
		else if(tok_stream_[i].type_ == OPERATOR){
			if(tok_stream_[i].token_.op == LPAREN){
				myAtom.type_ = SEXPR;
				myAtom.atom_ = (void *)(sres.first);
				expr.logical_not_.push_back(myAtom);
			}
			else if(tok_stream_[i].token_.op == LCURL){
				myAtom.type_ = LEXPR;
				myAtom.atom_ = (void *)(lres.first);
				expr.logical_not_.push_back(myAtom);
			}
			else if(tok_stream_[i].token_.op == delimiter_){
				return ++i;
			}
			else{
				I_error e("WRONG FORMAT OF ARITHMATIC EXPRESSION, AMBIGUOUS");
				e.print();
			}
		}
		else{
			myAtom.type_ = TOKEN;
			Token *myTok = new Token(tok_stream_[i]);
			myAtom.atom_ = (void *)myTok;
			expr.logical_not_.push_back(myAtom);
		}
		//cout<<"\n\n"<<j<<"\n\n";
	}
	return j;
}


pair<Logical_xor, size_t> Logical_xorParser::getLogical_xor(){
	pair<Logical_xor, size_t> ret;
	Logical_xor expr;
	ret.first = expr;
	ret.second = parse_(0, ret.first);
	destruct();
	return ret;
}

size_t Logical_xorParser::parse_(size_t begin, Logical_xor &expr){
	Logical_notParser lnParser;
	pair<Logical_not, size_t> lnres;

	size_t i=begin;
	lnParser.construct(tok_stream_, i);
	lnParser.setDelimiter(delimiter_);
	lnres = lnParser.getLogical_not();

	i += lnres.second;
	expr.logical_xor_.push_back(lnres.first);
	

	while(tok_stream_[i].type_ == OPERATOR){
		if(tok_stream_[i].token_.op == XOR){
			lnParser.construct(tok_stream_, ++i);
			lnParser.setDelimiter(delimiter_);
			lnres = lnParser.getLogical_not();
			
			i+= lnres.second;
			expr.logical_xor_.push_back(lnres.first);
		}
		else break;
	}

	return i;
}


pair<Logical_and, size_t> Logical_andParser::getLogical_and(){
	pair<Logical_and, size_t> ret;
	Logical_and expr;
	ret.first = expr;
	ret.second = parse_(0, ret.first);
	destruct();
	return ret;
}

size_t Logical_andParser::parse_(size_t begin, Logical_and &expr){
	Logical_xorParser lxParser;
	pair<Logical_xor, size_t> lxres;

	size_t i=begin;
	lxParser.construct(tok_stream_, i);
	lxParser.setDelimiter(delimiter_);
	lxres = lxParser.getLogical_xor();

	i += lxres.second;
	expr.logical_and_.push_back(lxres.first);
	

	while(tok_stream_[i].type_ == OPERATOR){
		if(tok_stream_[i].token_.op == AND){
			lxParser.construct(tok_stream_, ++i);
			lxParser.setDelimiter(delimiter_);
			lxres = lxParser.getLogical_xor();
			
			i+= lxres.second;
			expr.logical_and_.push_back(lxres.first);
		}
		else break;
	}

	return i;
}

pair<Logical_or, size_t> Logical_orParser::getLogical_or(){
	pair<Logical_or, size_t> ret;
	Logical_or expr;
	ret.first = expr;
	ret.second = parse_(0, ret.first);
	destruct();
	return ret;
}

size_t Logical_orParser::parse_(size_t begin, Logical_or &expr){
	Logical_andParser loParser;
	pair<Logical_and, size_t> lores;

	size_t i=begin;
	loParser.construct(tok_stream_, i);
	loParser.setDelimiter(delimiter_);
	lores = loParser.getLogical_and();

	i += lores.second;
	expr.logical_or_.push_back(lores.first);
	

	while(tok_stream_[i].type_ == OPERATOR){
		if(tok_stream_[i].token_.op == OR){
			loParser.construct(tok_stream_, ++i);
			loParser.setDelimiter(delimiter_);
			lores = loParser.getLogical_and();
			
			i+= lores.second;
			expr.logical_or_.push_back(lores.first);
		}
		else break;
	}

	return i;
}

pair<Unary, size_t> UnaryParser::getUnary(){
	pair<Unary, size_t> ret;
	Unary expr;
	ret.first = expr;
	ret.second = parse_(0, ret.first);
	destruct();
	return ret;
}

size_t UnaryParser::parse_(size_t begin, Unary &expr){
	Logical_orParser loParser;
	pair<Logical_or, size_t> lores;
	size_t i = begin;
	bool unary_op_ = false;

	if(tok_stream_[i].type_ == OPERATOR){
		if(tok_stream_[i].token_.op==PLUS || tok_stream_[i].token_.op==MINUS){
			unary_op_ = true;
			if(tok_stream_[i].token_.op == MINUS) expr.minus_ =true;
			loParser.construct(tok_stream_, ++i);
			loParser.setDelimiter(delimiter_);
			lores = loParser.getLogical_or();
			i += lores.second;
			expr.unary_.push_back(lores.first);
			return i;
		}
	}

	if(! unary_op_){
		loParser.construct(tok_stream_, i);
		loParser.setDelimiter(delimiter_);
		lores = loParser.getLogical_or();
		i += lores.second;
		expr.unary_.push_back(lores.first);
		return i;
	}
}


pair<Exponent, size_t> ExponentParser::getExponent(){
	pair<Exponent, size_t> ret;
	Exponent expr;
	ret.first = expr;
	ret.second = parse_(0, ret.first);
	destruct();
	return ret;
}

size_t ExponentParser::parse_(size_t begin, Exponent &expr){
	UnaryParser unParser;
	pair<Unary, size_t> unres;

	size_t i=begin;
	unParser.construct(tok_stream_, i);
	unParser.setDelimiter(delimiter_);
	unres = unParser.getUnary();

	i += unres.second;
	expr.exponent_.push_back(unres.first);
	

	while(tok_stream_[i].type_ == OPERATOR){
		if(tok_stream_[i].token_.op == POW){
			unParser.construct(tok_stream_, ++i);
			unParser.setDelimiter(delimiter_);
			unres = unParser.getUnary();
			
			i+= unres.second;
			expr.exponent_.push_back(unres.first);
		}
		else break;
	}

	return i;
}



pair<Modulus, size_t> ModulusParser::getModulus(){
	pair<Modulus, size_t> ret;
	Modulus expr;
	ret.first = expr;
	ret.second = parse_(0, ret.first);
	destruct();
	return ret;
}

size_t ModulusParser::parse_(size_t begin, Modulus &expr){
	ExponentParser exParser;
	pair<Exponent, size_t> exres;

	size_t i=begin;
	exParser.construct(tok_stream_, i);
	exParser.setDelimiter(delimiter_);
	exres = exParser.getExponent();

	i += exres.second;
	expr.modulus_.push_back(exres.first);
	

	while(tok_stream_[i].type_ == OPERATOR){
		if(tok_stream_[i].token_.op == MOD){
			exParser.construct(tok_stream_, ++i);
			exParser.setDelimiter(delimiter_);
			exres = exParser.getExponent();
			
			i+= exres.second;
			expr.modulus_.push_back(exres.first);
		}
		else break;
	}

	return i;
}

pair<FactorDiv, size_t> FactorDivParser::getFactorDiv(){
	pair<FactorDiv, size_t> ret;
	FactorDiv expr;
	ret.first = expr;
	ret.second = parse_(0, ret.first);
	destruct();
	return ret;
}

size_t FactorDivParser::parse_(size_t begin, FactorDiv &expr){
	ModulusParser moParser;
	pair<Modulus, size_t> mores;

	size_t i=begin;
	moParser.construct(tok_stream_, i);
	moParser.setDelimiter(delimiter_);
	mores = moParser.getModulus();

	i += mores.second;
	expr.factor_div_.push_back(mores.first);
	

	while(tok_stream_[i].type_ == OPERATOR){
		if(tok_stream_[i].token_.op == DIV){
			moParser.construct(tok_stream_, ++i);
			moParser.setDelimiter(delimiter_);
			mores = moParser.getModulus();
			
			i+= mores.second;
			expr.factor_div_.push_back(mores.first);
		}
		else break;
	}

	return i;
}


pair<FactorMult, size_t> FactorMultParser::getFactorMult(){
	pair<FactorMult, size_t> ret;
	FactorMult expr;
	ret.first = expr;
	ret.second = parse_(0, ret.first);
	destruct();
	return ret;
}

size_t FactorMultParser::parse_(size_t begin, FactorMult &expr){
	FactorDivParser fdParser;
	pair<FactorDiv, size_t> fdres;

	size_t i=begin;

	fdParser.construct(tok_stream_, i);
	fdParser.setDelimiter(delimiter_);
	fdres = fdParser.getFactorDiv();

	i += fdres.second;
	expr.factor_mult_.push_back(fdres.first);
	
	while(tok_stream_[i].type_ == OPERATOR){
		if(tok_stream_[i].token_.op == MULT){
			fdParser.construct(tok_stream_, ++i);
			fdParser.setDelimiter(delimiter_);
			fdres = fdParser.getFactorDiv();
			
			i+= fdres.second;
			expr.factor_mult_.push_back(fdres.first);
		}
		else break;
	}

	return i;
}

pair<TermAdd, size_t> TermAddParser::getTermAdd(){
	pair<TermAdd, size_t> ret;
	TermAdd expr;
	ret.first = expr;
	ret.second = parse_(0, ret.first);
	destruct();
	return ret;
}

size_t TermAddParser::parse_(size_t begin, TermAdd &expr){
	FactorMultParser fmParser;
	pair<FactorMult, size_t> fmres;

	size_t i=begin;
	fmParser.construct(tok_stream_, i);
	fmParser.setDelimiter(delimiter_);
	fmres = fmParser.getFactorMult();

	i += fmres.second;
	expr.term_add_.push_back(fmres.first);
	
	while(tok_stream_[i].type_ == OPERATOR){
		if(tok_stream_[i].token_.op == PLUS){

			fmParser.construct(tok_stream_, ++i);
			fmParser.setDelimiter(delimiter_);
			fmres = fmParser.getFactorMult();
			
			i+= fmres.second;
			expr.term_add_.push_back(fmres.first);
		}
		else break;
	}

	return i;
}

pair<TermSub, size_t> TermSubParser::getTermSub(){
	pair<TermSub, size_t> ret;
	TermSub expr;
	ret.first = expr;
	ret.second = parse_(0, ret.first);
	destruct();
	return ret;
}

size_t TermSubParser::parse_(size_t begin, TermSub &expr){
	TermAddParser taParser;
	pair<TermAdd, size_t> tares;

	size_t i=begin;
	taParser.construct(tok_stream_, i);
	taParser.setDelimiter(delimiter_);
	tares = taParser.getTermAdd();

	i += tares.second;
	expr.term_sub_.push_back(tares.first);
	

	while(tok_stream_[i].type_ == OPERATOR){
		if(tok_stream_[i].token_.op == MINUS){
			taParser.construct(tok_stream_, ++i);
			taParser.setDelimiter(delimiter_);
			tares = taParser.getTermAdd();
			
			i+= tares.second;
			expr.term_sub_.push_back(tares.first);
		}
		else break;
	}

	return i;
}

pair<Compare_Gt, size_t> Compare_GtParser::getCompare_Gt(){
	pair<Compare_Gt, size_t> ret;
	Compare_Gt expr;
	ret.first = expr;
	ret.second = parse_(0, ret.first);
	destruct();
	return ret;
}

size_t Compare_GtParser::parse_(size_t begin, Compare_Gt &expr){
	TermSubParser tsParser;
	pair<TermSub, size_t> tsres;

	size_t i=begin;
	tsParser.construct(tok_stream_, i);
	tsParser.setDelimiter(delimiter_);
	tsres = tsParser.getTermSub();

	i += tsres.second;
	expr.compare_gt_.push_back(tsres.first);
	

	while(tok_stream_[i].type_ == OPERATOR){
		if(tok_stream_[i].token_.op == GT){
			tsParser.construct(tok_stream_, ++i);
			tsParser.setDelimiter(delimiter_);
			tsres = tsParser.getTermSub();
			
			i+= tsres.second;
			expr.compare_gt_.push_back(tsres.first);
		}
		else break;
	}

	return i;
}

pair<Compare_Lt, size_t> Compare_LtParser::getCompare_Lt(){
	pair<Compare_Lt, size_t> ret;
	Compare_Lt expr;
	ret.first = expr;
	ret.second = parse_(0, ret.first);
	destruct();
	return ret;
}

size_t Compare_LtParser::parse_(size_t begin, Compare_Lt &expr){
	Compare_GtParser gtParser;
	pair<Compare_Gt, size_t> gtres;

	size_t i=begin;
	gtParser.construct(tok_stream_, i);
	gtParser.setDelimiter(delimiter_);
	gtres = gtParser.getCompare_Gt();

	i += gtres.second;
	expr.compare_lt_.push_back(gtres.first);
	

	while(tok_stream_[i].type_ == OPERATOR){
		if(tok_stream_[i].token_.op == LT){
			gtParser.construct(tok_stream_, ++i);
			gtParser.setDelimiter(delimiter_);
			gtres = gtParser.getCompare_Gt();
			
			i+= gtres.second;
			expr.compare_lt_.push_back(gtres.first);
		}
		else break;
	}

	return i;
}

pair<Compare_Gte, size_t> Compare_GteParser::getCompare_Gte(){
	pair<Compare_Gte, size_t> ret;
	Compare_Gte expr;
	ret.first = expr;
	ret.second = parse_(0, ret.first);
	destruct();
	return ret;
}

size_t Compare_GteParser::parse_(size_t begin, Compare_Gte &expr){
	Compare_LtParser ltParser;
	pair<Compare_Lt, size_t> ltres;

	size_t i=begin;
	ltParser.construct(tok_stream_, i);
	ltParser.setDelimiter(delimiter_);
	ltres = ltParser.getCompare_Lt();

	i += ltres.second;
	expr.compare_gte_.push_back(ltres.first);
	

	while(tok_stream_[i].type_ == OPERATOR){
		if(tok_stream_[i].token_.op == GTE){
			ltParser.construct(tok_stream_, ++i);
			ltParser.setDelimiter(delimiter_);
			ltres = ltParser.getCompare_Lt();
			
			i+= ltres.second;
			expr.compare_gte_.push_back(ltres.first);
		}
		else break;
	}

	return i;
}

pair<Compare_Lte, size_t> Compare_LteParser::getCompare_Lte(){
	pair<Compare_Lte, size_t> ret;
	Compare_Lte expr;
	ret.first = expr;
	ret.second = parse_(0, ret.first);
	destruct();
	return ret;
}

size_t Compare_LteParser::parse_(size_t begin, Compare_Lte &expr){
	Compare_GteParser geParser;
	pair<Compare_Gte, size_t> geres;

	size_t i=begin;
	geParser.construct(tok_stream_, i);
	geParser.setDelimiter(delimiter_);
	geres = geParser.getCompare_Gte();

	i += geres.second;
	expr.compare_lte_.push_back(geres.first);
	

	while(tok_stream_[i].type_ == OPERATOR){
		if(tok_stream_[i].token_.op == LTE){
			geParser.construct(tok_stream_, ++i);
			geParser.setDelimiter(delimiter_);
			geres = geParser.getCompare_Gte();
			
			i+= geres.second;
			expr.compare_lte_.push_back(geres.first);
		}
		else break;
	}

	return i;
}

pair<Compare_Neq, size_t> Compare_NeqParser::getCompare_Neq(){
	pair<Compare_Neq, size_t> ret;
	Compare_Neq expr;
	ret.first = expr;
	ret.second = parse_(0, ret.first);
	destruct();
	return ret;
}

size_t Compare_NeqParser::parse_(size_t begin, Compare_Neq &expr){
	Compare_LteParser leParser;
	pair<Compare_Lte, size_t> leres;

	size_t i=begin;
	leParser.construct(tok_stream_, i);
	leParser.setDelimiter(delimiter_);
	leres = leParser.getCompare_Lte();

	i += leres.second;
	expr.compare_neq_.push_back(leres.first);
	

	while(tok_stream_[i].type_ == OPERATOR){
		if(tok_stream_[i].token_.op == NEQ){
			leParser.construct(tok_stream_, ++i);
			leParser.setDelimiter(delimiter_);
			leres = leParser.getCompare_Lte();
			
			i+= leres.second;
			expr.compare_neq_.push_back(leres.first);
		}
		else break;
	}

	return i;
}

pair<Compare_Eq, size_t> Compare_EqParser::getCompare_Eq(){
	pair<Compare_Eq, size_t> ret;
	Compare_Eq expr;
	ret.first = expr;
	ret.second = parse_(0, ret.first);
	destruct();
	return ret;
}

size_t Compare_EqParser::parse_(size_t begin, Compare_Eq &expr){
	Compare_NeqParser neParser;
	pair<Compare_Neq, size_t> neres;

	size_t i=begin;
	neParser.construct(tok_stream_, i);
	neParser.setDelimiter(delimiter_);
	neres = neParser.getCompare_Neq();

	i += neres.second;
	expr.compare_eq_.push_back(neres.first);
	

	while(tok_stream_[i].type_ == OPERATOR){
		if(tok_stream_[i].token_.op == EQ){
			neParser.construct(tok_stream_, ++i);
			neParser.setDelimiter(delimiter_);
			neres = neParser.getCompare_Neq();
			
			i+= neres.second;
			expr.compare_eq_.push_back(neres.first);
		}
		else break;
	}

	return i;
}


/**


                                 ______________
                           ,===:'.,            `-._
                                `:.`---.__         `-._
                                  `:.     `--.         `.
                                    \.        `.         `.
                            (,,(,    \.         `.   ____,-`.,
                         (,'     `/   \.   ,--.___`.'
                     ,  ,'  ,--.  `,   \.;'         `
                      `{D, {    \  :    \;
                        V,,'    /  /    //
                        j;;    /  ,' ,-//.    ,---.      ,
                        \;'   /  ,' /  _  \  /  _  \   ,'/
                              \   `'  / \  `'  / \  `.' /
                               `.___,'   `.__,'   `.__,'  


                               HERE BE DRAGONS

*/


pair<Component, size_t> ComponentParser::getComponent(){
	pair<Component, size_t> ret;
	Component expr;
	ret.first = expr;
	ret.second = parse_(0, ret.first);
	destruct();
	return ret;
}

size_t ComponentParser::parse_(size_t begin, Component &expr){
	Compare_EqParser eqParser;
	pair<Compare_Eq, size_t> eqres;
	size_t i = begin;
	eqParser.construct(tok_stream_, i);
	eqParser.setDelimiter(delimiter_);

	eqres = eqParser.getCompare_Eq();
	i += eqres.second;
	expr.component_.push_back(eqres.first);

	if(tok_stream_[i].type_ == OPERATOR){
		if(tok_stream_[i].token_.op == COND){
			eqParser.construct(tok_stream_, ++i);
			eqParser.setDelimiter(delimiter_);
			eqres = eqParser.getCompare_Eq();
			i += eqres.second;
			expr.component_.push_back(eqres.first);

			if(tok_stream_[i].type_ == OPERATOR){
				if(tok_stream_[i].token_.op == ELSE){
					eqParser.construct(tok_stream_, ++i);
					eqParser.setDelimiter(delimiter_);
					eqres = eqParser.getCompare_Eq();
					expr.component_.push_back(eqres.first);
					//eqres.first.print();
					//cout<<"\n\n";
					i += eqres.second;

				}
			}
		}
	}
	return i;
}


pair<LExpr *, size_t> LExprParser::getLExpr(){
	pair<LExpr *, size_t> ret;
	ret.first = new LExpr;
	ret.second = parse_(0, ret.first);
	destruct();
	return ret;
}

size_t LExprParser::parse_(size_t begin, LExpr *lexpr){
	ComponentParser coParser;
	pair<Component, size_t> cores;
	size_t i = begin;

	coParser.construct(tok_stream_, i);
	coParser.setDelimiter(delimiter_);
	cores = coParser.getComponent();
	i += cores.second;
	lexpr->lexpr_.push_back(cores.first);

	while(tok_stream_[i].type_ == OPERATOR){
		if(tok_stream_[i].token_.op == COMMA){
			coParser.construct(tok_stream_, ++i);
			coParser.setDelimiter(delimiter_);
			cores = coParser.getComponent();
			
			i += cores.second;
			lexpr->lexpr_.push_back(cores.first);
		}
		else if(tok_stream_[i].token_.op == delimiter_)
			return i;
		else break;
	}
	return i;
}


void atom::print(){
	Token *tok;
	SExpr *sexpr;
	LExpr *lexpr;
	Variable *var;
	cout<<"<ATOM>"<<endl;


	if(indexed_){
		index_.print();
	}


	switch(type_){
		case TOKEN:
			//cout<<"<ATOM>"<<endl;
			tok = (Token *)atom_;
			tok->print();
			//cout<<"</ATOM>"<<endl;
			break;
		case SEXPR:
			//cout<<"<S-EXPR>"<<endl;
			sexpr = (SExpr *)atom_;
			sexpr->print();
			//cout<<"</S-EXPR>"<<endl;
			break;
		case LEXPR:
			//cout<<"<L-EXPR>"<<endl;
			lexpr = (LExpr *)atom_;
			lexpr->print();
			//cout<<"</L-EXPR>"<<endl;
			break;
		case VARIABLE:
			//cout<<"<VARIABLE>"<<endl;
			var = (Variable *)atom_;
			var->print();
			//cout<<"</VARIABLE>"<<endl;
			break;
		default:
			cout<<"<ERROR/> "<<endl;
	}
	cout<<"</ATOM>"<<endl;
}
#ifdef PARSE_OUT_DOT_ 
void atom::printDot(int parent){
	Token *tok;
	SExpr *sexpr;
	LExpr *lexpr;
	Variable *var;

	if(indexed_){
		sstream nodeName("node_");
		sstream parName("node_");
		nodeName << node_id_;
		parName << parent;
		
		cout<<parName.str()<<"->"<<nodeName.str()<<";"<<endl;
		cout<<nodeName.str()<<"[shape=box, fontsize=14, fontname=\"Consolas\", "
			<<"label=\"ATOM\"];"<<endl;
		
		index_.printDot(node_id_);
		parent = node_id_;

	}

	switch(type_){
		case TOKEN:
			tok = (Token *)atom_;
			tok->printDot(parent);
			break;
		case SEXPR:
			sexpr = (SExpr *)atom_;
			sexpr->printDot(parent);
			break;
		case LEXPR:
			lexpr = (LExpr *)atom_;
			lexpr->printDot(parent);
			break;
		case VARIABLE:
			var = (Variable *)atom_;
			var->printDot(parent);
			break;
		default:
			cout<<"----ERROR----"<<endl;
	}
}

#endif

void Logical_not::print(){
	cout<<"<LOGICAL_NOT is_not=\""<<(not_?"true":"false")<<"\">"<<endl;
	for(size_t i=0; i<logical_not_.size(); i++)
		logical_not_[i].print();
	cout<<"</LOGICAL_NOT>"<<endl;
}

#ifdef PARSE_OUT_DOT_
void Logical_not::printDot(int parent){
	if(logical_not_.size()==1 && not_){
		sstream nodeName("node_");
		sstream parName("node_");
		nodeName << node_id_;
		parName << parent;
		
		cout<<parName.str()<<"->"<<nodeName.str()<<";"<<endl;
		cout<<nodeName.str()<<"[shape=box, fontsize=14, fontname=\"Consolas\", "
			<<"label=\"NOT\\n~\"];"<<endl;
		
		logical_not_[0].printDot(node_id_);
	}
	else{
		for(size_t i=0; i<logical_not_.size();i++)
			logical_not_[i].printDot(parent);
	}
}
#endif

void Logical_xor::print(){
	cout<<"<LOGICAL_XOR>"<<endl;
	for(size_t i=0; i<logical_xor_.size(); i++)
		logical_xor_[i].print();
	cout<<"</LOGICAL_XOR>"<<endl;
}

#ifdef PARSE_OUT_DOT_
void Logical_xor::printDot(int parent){
	if(logical_xor_.size() > 1){
		sstream nodeName("node_");
		sstream parName("node_");
		nodeName << node_id_;
		parName << parent;
		
		cout<<parName.str()<<"->"<<nodeName.str()<<";"<<endl;
		cout<<nodeName.str()<<"[shape=box, fontsize=14, fontname=\"Consolas\", "
			<<"label=\"XOR\\n^\"];"<<endl;
		
		for(size_t i=0; i<logical_xor_.size();i++)
			logical_xor_[i].printDot(node_id_);
	}
	else{
		for(size_t i=0; i<logical_xor_.size();i++)
			logical_xor_[i].printDot(parent);
	}
}
#endif

void Logical_and::print(){
	cout<<"<LOGICAL_AND>"<<endl;
	for(size_t i=0; i<logical_and_.size(); i++)
		logical_and_[i].print();
	cout<<"</LOGICAL_AND>"<<endl;
}

#ifdef PARSE_OUT_DOT_
void Logical_and::printDot(int parent){
	if(logical_and_.size() > 1){
		sstream nodeName("node_");
		sstream parName("node_");
		nodeName << node_id_;
		parName << parent;
		
		cout<<parName.str()<<"->"<<nodeName.str()<<";"<<endl;
		cout<<nodeName.str()<<"[shape=box, fontsize=14, fontname=\"Consolas\", "
			<<"label=\"AND\\n&\"];"<<endl;
		
		for(size_t i=0; i<logical_and_.size();i++)
			logical_and_[i].printDot(node_id_);
	}
	else{
		for(size_t i=0; i<logical_and_.size();i++)
			logical_and_[i].printDot(parent);
	}
}
#endif


void Logical_or::print(){
	cout<<"<LOGICAL_OR>"<<endl;
	for(size_t i=0; i<logical_or_.size(); i++)
		logical_or_[i].print();
	cout<<"</LOGICAL_OR>"<<endl;
}

#ifdef PARSE_OUT_DOT_
void Logical_or::printDot(int parent){
	if(logical_or_.size() > 1){
		sstream nodeName("node_");
		sstream parName("node_");
		nodeName << node_id_;
		parName << parent;
		
		cout<<parName.str()<<"->"<<nodeName.str()<<";"<<endl;
		cout<<nodeName.str()<<"[shape=box, fontsize=14, fontname=\"Consolas\", "
			<<"label=\"OR\\n|\"];"<<endl;
		
		for(size_t i=0; i<logical_or_.size();i++)
			logical_or_[i].printDot(node_id_);
	}
	else{
		for(size_t i=0; i<logical_or_.size();i++)
			logical_or_[i].printDot(parent);
	}
}
#endif

void Unary::print(){
	cout<<"<UNARY-OPERATOR is_minus=\""<<(minus_?"true":"false")<<"\">"<<endl;
	for(size_t i=0; i<unary_.size(); i++)
		unary_[i].print();
	cout<<"</UNARY-OPERATOR>"<<endl;
}

#ifdef PARSE_OUT_DOT_
void Unary::printDot(int parent){
	if(unary_.size()==1 && minus_){
		sstream nodeName("node_");
		sstream parName("node_");
		nodeName << node_id_;
		parName << parent;
		
		cout<<parName.str()<<"->"<<nodeName.str()<<";"<<endl;
		cout<<nodeName.str()<<"[shape=box, fontsize=14, fontname=\"Consolas\", "
			<<"label=\"NEGATIVE\\n-\"];"<<endl;
		
		unary_[0].printDot(node_id_);
	}
	else{
		for(size_t i=0; i<unary_.size();i++)
			unary_[i].printDot(parent);
	}
}
#endif

void Exponent::print(){
	cout<<"<EXPONENT>"<<endl;
	for(size_t i=0; i<exponent_.size(); i++)
		exponent_[i].print();
	cout<<"</EXPONENT>"<<endl;
}

#ifdef PARSE_OUT_DOT_
void Exponent::printDot(int parent){
	if(exponent_.size() > 1){
		sstream nodeName("node_");
		sstream parName("node_");
		nodeName << node_id_;
		parName << parent;
		
		cout<<parName.str()<<"->"<<nodeName.str()<<";"<<endl;
		cout<<nodeName.str()<<"[shape=box, fontsize=14, fontname=\"Consolas\", "
			<<"label=\"POW\\n**\"];"<<endl;
		
		for(size_t i=0; i<exponent_.size();i++)
			exponent_[i].printDot(node_id_);
	}
	else{
		for(size_t i=0; i<exponent_.size();i++)
			exponent_[i].printDot(parent);
	}
}
#endif

void Modulus::print(){
	cout<<"<MODULUS>"<<endl;
	for(size_t i=0; i<modulus_.size(); i++)
		modulus_[i].print();
	cout<<"</MODULUS>"<<endl;
}

#ifdef PARSE_OUT_DOT_
void Modulus::printDot(int parent){
	if(modulus_.size() > 1){
		sstream nodeName("node_");
		sstream parName("node_");
		nodeName << node_id_;
		parName << parent;
		
		cout<<parName.str()<<"->"<<nodeName.str()<<";"<<endl;
		cout<<nodeName.str()<<"[shape=box, fontsize=14, fontname=\"Consolas\", "
			<<"label=\"MOD\\n%\"];"<<endl;
		
		for(size_t i=0; i<modulus_.size();i++)
			modulus_[i].printDot(node_id_);
	}
	else{
		for(size_t i=0; i<modulus_.size();i++)
			modulus_[i].printDot(parent);
	}
}
#endif

void FactorDiv::print(){
	cout<<"<DIVISION>"<<endl;
	for(size_t i=0; i<factor_div_.size(); i++)
		factor_div_[i].print();
	cout<<"</DIVISION>"<<endl;
}

#ifdef PARSE_OUT_DOT_
void FactorDiv::printDot(int parent){
	if(factor_div_.size() > 1){
		sstream nodeName("node_");
		sstream parName("node_");
		nodeName << node_id_;
		parName << parent;
		
		cout<<parName.str()<<"->"<<nodeName.str()<<";"<<endl;
		cout<<nodeName.str()<<"[shape=box, fontsize=14, fontname=\"Consolas\", "
			<<"label=\"DIVISION\\n/\"];"<<endl;
		
		for(size_t i=0; i<factor_div_.size();i++)
			factor_div_[i].printDot(node_id_);
	}
	else{
		for(size_t i=0; i<factor_div_.size();i++)
			factor_div_[i].printDot(parent);
	}
}
#endif

void FactorMult::print(){
	cout<<"<MULTIPLICATION>"<<endl;
	for(size_t i=0; i<factor_mult_.size(); i++)
		factor_mult_[i].print();
	cout<<"</MULTIPLICATION>"<<endl;
}

#ifdef PARSE_OUT_DOT_
void FactorMult::printDot(int parent){
	if(factor_mult_.size() > 1){
		sstream nodeName("node_");
		sstream parName("node_");
		nodeName << node_id_;
		parName << parent;
		
		cout<<parName.str()<<"->"<<nodeName.str()<<";"<<endl;
		cout<<nodeName.str()<<"[shape=box, fontsize=14, fontname=\"Consolas\", "
			<<"label=\"MULTIPLICATION\\n*\"];"<<endl;
		
		for(size_t i=0; i<factor_mult_.size();i++)
			factor_mult_[i].printDot(node_id_);
	}
	else{
		for(size_t i=0; i<factor_mult_.size();i++)
			factor_mult_[i].printDot(parent);
	}
}
#endif

void TermAdd::print(){
	cout<<"<ADDITION>"<<endl;
	for(size_t i=0; i<term_add_.size(); i++)
		term_add_[i].print();
	cout<<"</ADDITION>"<<endl;
}

#ifdef PARSE_OUT_DOT_
void TermAdd::printDot(int parent){
	if(term_add_.size() > 1){
		sstream nodeName("node_");
		sstream parName("node_");
		nodeName << node_id_;
		parName << parent;
		
		cout<<parName.str()<<"->"<<nodeName.str()<<";"<<endl;
		cout<<nodeName.str()<<"[shape=box, fontsize=14, fontname=\"Consolas\", "
			<<"label=\"ADDITION\\n+\"];"<<endl;
		
		for(size_t i=0; i<term_add_.size();i++)
			term_add_[i].printDot(node_id_);
	}
	else{
		for(size_t i=0; i<term_add_.size();i++)
			term_add_[i].printDot(parent);
	}
}
#endif

void TermSub::print(){
	cout<<"<SUBTRACTION>"<<endl;
	for(size_t i=0; i<term_sub_.size(); i++)
		term_sub_[i].print();
	cout<<"</SUBTRACTION>"<<endl;
}

#ifdef PARSE_OUT_DOT_
void TermSub::printDot(int parent){
	if(term_sub_.size() > 1){
		sstream nodeName("node_");
		sstream parName("node_");
		nodeName << node_id_;
		parName << parent;
		
		cout<<parName.str()<<"->"<<nodeName.str()<<";"<<endl;
		cout<<nodeName.str()<<"[shape=box, fontsize=14, fontname=\"Consolas\", "
			<<"label=\"SUBTRACTION\\n-\"];"<<endl;
		
		for(size_t i=0; i<term_sub_.size();i++)
			term_sub_[i].printDot(node_id_);
	}
	else{
		for(size_t i=0; i<term_sub_.size();i++)
			term_sub_[i].printDot(parent);
	}
}
#endif

void Compare_Gt::print(){
	cout<<"<GT>"<<endl;
	for(size_t i=0; i<compare_gt_.size(); i++)
		compare_gt_[i].print();
	cout<<"</GT>"<<endl;
}

#ifdef PARSE_OUT_DOT_
void Compare_Gt::printDot(int parent){
	if(compare_gt_.size() > 1){
		sstream nodeName("node_");
		sstream parName("node_");
		nodeName << node_id_;
		parName << parent;
		
		cout<<parName.str()<<"->"<<nodeName.str()<<";"<<endl;
		cout<<nodeName.str()<<"[shape=box, fontsize=14, fontname=\"Consolas\", "
			<<"label=\"GT\\n>\"];"<<endl;
		
		for(size_t i=0; i<compare_gt_.size();i++)
			compare_gt_[i].printDot(node_id_);
	}
	else{
		for(size_t i=0; i<compare_gt_.size();i++)
			compare_gt_[i].printDot(parent);
	}
}
#endif

void Compare_Lt::print(){
	cout<<"<LT>"<<endl;
	for(size_t i=0; i<compare_lt_.size(); i++)
		compare_lt_[i].print();
	cout<<"</LT>"<<endl;
}

#ifdef PARSE_OUT_DOT_
void Compare_Lt::printDot(int parent){
	if(compare_lt_.size() > 1){
		sstream nodeName("node_");
		sstream parName("node_");
		nodeName << node_id_;
		parName << parent;
		
		cout<<parName.str()<<"->"<<nodeName.str()<<";"<<endl;
		cout<<nodeName.str()<<"[shape=box, fontsize=14, fontname=\"Consolas\", "
			<<"label=\"LT\\n<\"];"<<endl;
		
		for(size_t i=0; i<compare_lt_.size();i++)
			compare_lt_[i].printDot(node_id_);
	}
	else{
		for(size_t i=0; i<compare_lt_.size();i++)
			compare_lt_[i].printDot(parent);
	}
}
#endif

void Compare_Gte::print(){
	cout<<"<GTE>"<<endl;
	for(size_t i=0; i<compare_gte_.size(); i++)
		compare_gte_[i].print();
	cout<<"</GTE>"<<endl;
}

#ifdef PARSE_OUT_DOT_
void Compare_Gte::printDot(int parent){
	if(compare_gte_.size() > 1){
		sstream nodeName("node_");
		sstream parName("node_");
		nodeName << node_id_;
		parName << parent;
		
		cout<<parName.str()<<"->"<<nodeName.str()<<";"<<endl;
		cout<<nodeName.str()<<"[shape=box, fontsize=14, fontname=\"Consolas\", "
			<<"label=\"GTE\\n>=\"];"<<endl;
		
		for(size_t i=0; i<compare_gte_.size();i++)
			compare_gte_[i].printDot(node_id_);
	}
	else{
		for(size_t i=0; i<compare_gte_.size();i++)
			compare_gte_[i].printDot(parent);
	}
}
#endif

void Compare_Lte::print(){
	cout<<"<LTE>"<<endl;
	for(size_t i=0; i<compare_lte_.size(); i++)
		compare_lte_[i].print();
	cout<<"</LTE>"<<endl;
}

#ifdef PARSE_OUT_DOT_
void Compare_Lte::printDot(int parent){
	if(compare_lte_.size() > 1){
		sstream nodeName("node_");
		sstream parName("node_");
		nodeName << node_id_;
		parName << parent;
		
		cout<<parName.str()<<"->"<<nodeName.str()<<";"<<endl;
		cout<<nodeName.str()<<"[shape=box, fontsize=14, fontname=\"Consolas\", "
			<<"label=\"LTE\\n<=\"];"<<endl;
		
		for(size_t i=0; i<compare_lte_.size();i++)
			compare_lte_[i].printDot(node_id_);
	}
	else{
		for(size_t i=0; i<compare_lte_.size();i++)
			compare_lte_[i].printDot(parent);
	}
}
#endif

void Compare_Neq::print(){
	cout<<"<NEQ>"<<endl;
	for(size_t i=0; i<compare_neq_.size(); i++)
		compare_neq_[i].print();
	cout<<"</NEQ>"<<endl;
}

#ifdef PARSE_OUT_DOT_
void Compare_Neq::printDot(int parent){
	if(compare_neq_.size() > 1){
		sstream nodeName("node_");
		sstream parName("node_");
		nodeName << node_id_;
		parName << parent;
		
		cout<<parName.str()<<"->"<<nodeName.str()<<";"<<endl;
		cout<<nodeName.str()<<"[shape=box, fontsize=14, fontname=\"Consolas\", "
			<<"label=\"NOT EQUAL\\n~=\"];"<<endl;
		
		for(size_t i=0; i<compare_neq_.size();i++)
			compare_neq_[i].printDot(node_id_);
	}
	else{
		for(size_t i=0; i<compare_neq_.size();i++)
			compare_neq_[i].printDot(parent);
	}
}
#endif

void Compare_Eq::print(){
	cout<<"<EQ>"<<endl;
	for(size_t i=0; i<compare_eq_.size(); i++)
		compare_eq_[i].print();
	cout<<"</EQ>"<<endl;
}


#ifdef PARSE_OUT_DOT_
void Compare_Eq::printDot(int parent){
	if(compare_eq_.size() > 1){
		sstream nodeName("node_");
		sstream parName("node_");
		nodeName << node_id_;
		parName << parent;
		
		cout<<parName.str()<<"->"<<nodeName.str()<<";"<<endl;
		cout<<nodeName.str()<<"[shape=box, fontsize=14, fontname=\"Consolas\", "
			<<"label=\"EQUAL\\n==\"];"<<endl;
		
		for(size_t i=0; i<compare_eq_.size();i++)
			compare_eq_[i].printDot(node_id_);
	}
	else{
		for(size_t i=0; i<compare_eq_.size();i++)
			compare_eq_[i].printDot(parent);
	}
}
#endif

void Component::print(){
	cout<<"<CONDITIONAL>"<<endl;
	if(component_.size()==0){
		cout<<"<ERROR/>"<<endl;
	}
	else if(component_.size()==1){
		cout<<"<ALWAYS>"<<endl;
		component_[0].print();
		cout<<"</ALWAYS>"<<endl;
	}
	else if(component_.size()==2){	
		cout<<"<CONDITION>"<<endl;
		component_[0].print();
		cout<<"</CONDITION>"<<endl;
		
		cout<<"<SATISFIED>"<<endl;
		component_[1].print();
		cout<<"</SATISFIED>"<<endl;
	}
	else if(component_.size()==3){	
		cout<<"<CONDITION>"<<endl;
		component_[0].print();
		cout<<"</CONDITION>"<<endl;
		
		cout<<"<SATISFIED>"<<endl;
		component_[1].print();
		cout<<"</SATISFIED>"<<endl;
		
		cout<<"<ELSE>"<<endl;
		component_[2].print();
		cout<<"</ELSE>"<<endl;
	}
	else{
		cout<<"<FAULTY-CONDITIONAL>"<<endl;
		for(size_t i=0; i<component_.size(); i++)
			component_[i].print();	
		cout<<"</FAULTY-CONDITIONAL>"<<endl;
	}
	cout<<"</CONDITIONAL>"<<endl;
}

#ifdef PARSE_OUT_DOT_
void Component::printDot(int parent){
	if(component_.size() == 0){
		cout<<"----ERROR----"<<endl;
	}
	else if(component_.size()==1){
		component_[0].printDot(parent);
	}
	else if(component_.size() >= 2 && component_.size() < 4){
		sstream nodeName("node_");
		sstream parName("node_");
		nodeName << node_id_;
		parName << parent;
		
		cout<<parName.str()<<"->"<<nodeName.str()<<";"<<endl;
		cout<<nodeName.str()<<"[shape=diamond, regular=1, "
			<<"fontsize=14, fontname=\"Consolas\", "
			<<"label=\"IF\"];"<<endl;
		
		sstream condNode("node_");
		condNode << (nodeCounter++);
		cout<<condNode.str()<<"->"<<nodeName.str()<<"[style=dashed, minlen=2];"
			<<endl;
		cout<<condNode.str()<<"[regular=1,fontsize=14, fontname=\"Consolas\", "
			<<"label=\"CONDITION\"];"<<endl;
		component_[0].printDot(nodeCounter-1);
		cout<<"{rank=same; "<<condNode.str()<<" "<<nodeName.str()<<"}"<<endl;
		
		sstream yesNode("node_");
		yesNode << (nodeCounter++);
		cout<<nodeName.str()<<"->"<<yesNode.str()<<";"<<endl;
		cout<<yesNode.str()<<"[regular=1,fontsize=14, fontname=\"Consolas\", "
			<<"label=\"TRUE\"];"<<endl;
		component_[1].printDot(nodeCounter-1);

		if(component_.size()==3){
			sstream noNode("node_");
			noNode << (nodeCounter++);
			cout<<nodeName.str()<<"->"<<noNode.str()<<";"<<endl;
			cout<<noNode.str()<<"[regular=1,fontsize=14,fontname=\"Consolas\","
				<<"label=\"FALSE\"];"<<endl;
			component_[2].printDot(nodeCounter-1);
		}
	}
	else{
		for(size_t i=0; i<component_.size();i++)
			component_[i].printDot(parent);
	}
}
#endif


void LExpr::print(){
	cout<<"<L-EXPRESSION>"<<endl;
	for(size_t i=0; i<lexpr_.size(); i++)
		lexpr_[i].print();
	cout<<"</L-EXPRESSION>"<<endl;
}

#ifdef PARSE_OUT_DOT_
void LExpr::printDot(int parent){
	sstream nodeName("node_");
	sstream parName("node_");
	nodeName << node_id_;
	parName << parent;
	
	cout<<parName.str()<<"->"<<nodeName.str()<<";"<<endl;
	cout<<nodeName.str()<<"[shape=box, fontsize=14, fontname=\"Consolas\", "
		<<"label=\"L-EXPRESSION\"];"<<endl;
	
	for(size_t i=0; i<lexpr_.size();i++)
			lexpr_[i].printDot(node_id_);
}
#endif

void Variable::print(){
	cout<<"<VARIABLE>"<<endl;
	if(context_ != NULL){
		cout<<"<CONTEXT>"<<endl;
		context_->print();
		cout<<"</CONTEXT>"<<endl;
	}
	cout<<"<VAR-NAME>"<<var_name_<<"</VAR-NAME>"<<endl;
	cout<<"</VARIABLE>"<<endl;
}

#ifdef PARSE_OUT_DOT_
void Variable::printDot(int parent){
	sstream nodeName("node_");
	sstream parName("node_");
	nodeName << node_id_;
	parName << parent;

	cout<<parName.str()<<"->"<<nodeName.str()<<";"<<endl;
	cout<<nodeName.str()<<"[shape=box, fontsize=14, fontname=\"Consolas\", "
		<<"label=\"VARIABLE\\n"<<var_name_<<"\"];"<<endl;

	if(context_ != NULL){
		sstream contNode("node_");
		contNode << (nodeCounter++);
		cout<<contNode.str()<<"->"<<nodeName.str()<<"[style=dashed, minlen=2];"
			<<endl;
		cout<<contNode.str()<<"[regular=1,fontsize=14, fontname=\"Consolas\", "
			<<"label=\"CONTEXT\"];"<<endl;
		context_->printDot(nodeCounter-1);
		cout<<"{rank=same; "<<contNode.str()<<" "<<nodeName.str()<<"}"<<endl;
	}
}
#endif

void Indexed::print(){
	cout<<"<INDEX>"<<endl;
	if(index_ != NULL){
		index_->print();
	}
	else cout<<"_ALL"<<endl;
	cout<<"</INDEX>"<<endl;
}
#ifdef PARSE_OUT_DOT_
void Indexed::printDot(int parent){
	sstream nodeName("node_");
	sstream parName("node_");
	nodeName << node_id_;
	parName << parent;

	cout<<parName.str()<<"->"<<nodeName.str()<<";"<<endl;
	if(index_ != NULL){
		cout<<nodeName.str()<<"[shape=box, fontsize=14, fontname=\"Consolas\", "
			<<"label=\"INDEX\\n[]\"];"<<endl;
		index_->printDot(node_id_);
	}
	else cout<<nodeName.str()<<"[shape=box, fontsize=14, fontname=\"Consolas\", "
			<<"label=\"INDEX\\n[_ALL]\"];"<<endl;
}
#endif

void SExpr::print(){
	cout<<"<S-EXPRESSION>"<<endl;
	for(size_t i=0; i<sexpr_.size(); i++)
		sexpr_[i].print();
	cout<<"</S-EXPRESSION>"<<endl;
}

#ifdef PARSE_OUT_DOT_
void SExpr::printDot(int parent){
	sstream nodeName("node_");
	sstream parName("node_");
	nodeName << node_id_;
	parName << parent;
	
	cout<<parName.str()<<"->"<<nodeName.str()<<";"<<endl;
	cout<<nodeName.str()<<"[shape=box, fontsize=14, fontname=\"Consolas\", "
		<<"label=\"S-EXPRESSION\"];"<<endl;
	
	for(size_t i=0; i<sexpr_.size();i++)
			sexpr_[i].printDot(node_id_);
}
#endif


#ifdef PARSE_OUT_DOT_
void Statement::printDot(int parent){
	sstream nodeName("node_");
	nodeName << node_id_;

	cout<<nodeName.str()<<"[shape=box, fontsize=14, fontname=\"Consolas\", "
		<<"label=\"STATEMENT\\nACTION = "<<(store_?"STORE":"EVAL & PRINT")
		<<"\"];"<<endl;
	
	lval_->printDot(node_id_);
	if(store_) rval_->printDot(node_id_);
}
#endif
