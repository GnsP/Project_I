#ifndef SYM_TBL_H_
#define SYM_TBL_H_

#include "includes.hpp"

class SymTbl{
	public:
		typedef SymTbl type;
		typedef std::string key_type;
		typedef bit64 value_type;
		typedef bit64 count_type;

		SymTbl():offset(0),sym_count(0){}
		bool insert(const key_type key){
			if(table.count(key)) return false;
			table[key]=++sym_count;
			return true;
		}

		const value_type &operator [](const key_type &key) const{
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
		std::map<key_type, value_type> table;
};

#endif
