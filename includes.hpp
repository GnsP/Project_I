#ifndef I_INCLUDES_H_
#define I_INCLUDES_H_

#include <string>
#include <vector>
#include <map>
#include <utility>
#include <stack>
#include <algorithm>
#include <iostream>
#include <cstring>
#include <cctype>
#include <cstdlib>
#include <set>

#include <cstdio>
#include "sstream.hpp"

#include "graphconfig.hpp"

using namespace std;

typedef unsigned long long bit64;
typedef char bit8;
typedef unsigned int bit32;
typedef unsigned short bit16;

extern bit64 line_no;
extern bit64 col_no;
extern int indentLevel;

#ifdef PARSE_OUT_DOT_
extern int nodeCounter;
#endif

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

#endif
