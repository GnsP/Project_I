#include "includes.hpp"
#include "sym_tbl.hpp"

using namespace std;

class machine{
	public:
		typedef bit64 word_type;
		typedef bit8 byte_type;

		machine():current_state(0){}
		bool putStatement(stmt *s){
			

	private:
		word_type current_state;
		SymTbl sym_table;
		std::map<std::pair<word_type,word_type>,word_type> transition_table;
		std::set<word_type> final_states;
		std::vector<std::vector<sexpr *> > eval_rules;
		std::vector<word_type, size_t> binder_table;
};
