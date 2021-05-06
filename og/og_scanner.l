%option c++ prefix="og_scanner_" outfile="og_scanner.cpp"
%option stack noyywrap yylineno 8bit
%{ 
// make relevant includes before including the parser's tab file
#include <string>
#include <cdk/ast/sequence_node.h>
#include <cdk/ast/expression_node.h>
#include <cdk/ast/lvalue_node.h>
#include "og_parser.tab.h"

// don't change this
#define yyerror LexerError
%}
%x X_STRING X_COMMENT X_SLASH X_IGNORE
%%

"//".*                  ;    /* ignore explanatory comments */

"/*"                    yy_push_state(X_COMMENT);
<X_COMMENT>"/*"         yy_push_state(X_COMMENT);
<X_COMMENT>"*/"         yy_pop_state();
<X_COMMENT>.|\n         ;    /* ignore operational comments */

"int"                   return tINT_TYPE;
"real"                  return tREAL_TYPE;
"string"                return tSTRING_TYPE;
"auto"                  return tAUTO;

"ptr"                   return tPTR;
"nullptr"               return tNULLPTR;

"public"                return tPUBLIC;
"require"               return tREQUIRE;
"procedure"             return tPROCEDURE;

">="                    return tGE;
"<="                    return tLE;
"=="                    return tEQ;
"!="                    return tNE;
"&&"                    return tAND;
"||"                    return tOR;

"if"                    return tIF;
"then"                  return tTHEN;
"elif"                  return tELIF;
"else"                  return tELSE;
"for"                   return tFOR;
"do"                    return tDO;
"break"                 return tBREAK;
"continue"              return tCONTINUE;
"return"                return tRETURN;

"input"                 return tINPUT;
"write"                 return tWRITE;
"writeln"               return tWRITELN;

"sizeof"                return tSIZEOF;

[A-Za-z][A-Za-z0-9_]*   yylval.s = new std::string(yytext); return tIDENTIFIER;

\"                      yy_push_state(X_STRING); yylval.s = new std::string("");
<X_STRING>\\0[^A-F^a-f]   yy_push_state(X_IGNORE);return tSTRING; 
<X_STRING>\\            yy_push_state(X_SLASH);
<X_STRING>\"            yy_pop_state();return tSTRING;
<X_STRING>\\\"          *yylval.s += yytext + 1;
<X_STRING>.             *yylval.s += yytext;


<X_SLASH>n              *yylval.s += '\n'; yy_pop_state();
<X_SLASH>r              *yylval.s += '\r'; yy_pop_state();
<X_SLASH>t              *yylval.s += '\t'; yy_pop_state();
<X_SLASH>\"             *yylval.s += '"';  yy_pop_state();
<X_SLASH>\\             *yylval.s += '\\'; yy_pop_state();
<X_SLASH>[0-9A-Fa-f]{1,2} {
                        *yylval.s += std::stoi(yytext, nullptr, 16);
                        yy_pop_state();
                    }
<X_SLASH>.              *yylval.s += yytext; yy_pop_state();


<X_IGNORE>\\\"			;
<X_IGNORE>\"			yy_pop_state(); yy_pop_state();
<X_IGNORE>.				;
<X_IGNORE>\n			yyerror("newline in string");

0x[0-9A-Fa-f]+          {
			    try {
				yylval.i = std::stoi(yytext, nullptr, 16); return tINTEGER;
  			    }
  			    catch (const std::out_of_range& oor) {
    			    	yyerror("Out of Range error.");
 		 	    }
			}

[0-9]+          	{
			    try {
				yylval.i = std::stoi(yytext, nullptr, 10); return tINTEGER;
  			    }
  			    catch (const std::out_of_range& oor) {
    			    	yyerror("Out of Range error.");
 		 	    }
			}

[+-]?(([0-9]*[.][0-9]+|[0-9]+[.]?[0-9]*)([Ee]([-+])?[0-9]+)|([.][0-9]+)|([0-9]+)([.][0-9]*)) {
			    try {
				yylval.d = std::stod(yytext, nullptr); return tREAL;
		            }
  			    catch (const std::out_of_range& oor) {
    			    	yyerror("Out of Range error.");
 		 	    }
			}

[-()\[\]<>=+*/%;{}.,?@~] return *yytext;

[ \t\r\n]+              ; /* ignore whitespace */

.                       yyerror("Unknown character");

%%
