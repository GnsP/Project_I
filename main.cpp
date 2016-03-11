#include "parser.hpp"
using namespace std;

bit64 line_no = 1;
bit64 col_no = 0;
int indentLevel = 0;
int nodeCounter = 1;

int main(){
	char str[1024],tmp[1024];
	/*strcpy(str,"");
	while(scanf("%[^\n]",tmp)){
		if(tmp[strlen(tmp)-1]=='\\'){
			strcat(str,tmp);
		}
	}*/
	scanf("%[^\n]",str);
	//cout<<str;
	StatementParser parse(str);//"$s+{$[]a+$[~(-7),8,true?x:{y+z}]b}=5");
	Statement *expr = parse.getStmt();
#ifdef PARSE_OUT_DOT_
	cout<<"digraph G{nodesep=0.5;ordering=out;"<<endl;
	expr->printDot(0);
	cout<<"}"<<endl;
#endif

#ifdef PARSE_OUT_XML_
	expr->print();
#endif
	
	return 0;
}
