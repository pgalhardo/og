%{
//-- don't change *any* of these: if you do, you'll break the compiler.
#include <cdk/compiler.h>
#include "ast/all.h"
#define LINE               compiler->scanner()->lineno()
#define yylex()            compiler->scanner()->scan()
#define yyerror(s)         compiler->scanner()->error(s)
#define YYPARSE_PARAM_TYPE std::shared_ptr<cdk::compiler>
#define YYPARSE_PARAM      compiler
//-- don't change *any* of these --- END!
%}

%union {
  int                   i;	        /* integer value */
  double                d;              /* double value */
  std::string          *s;	        /* symbol name or string literal */
  cdk::basic_node      *node;	        /* node pointer */
  cdk::sequence_node   *sequence;
  cdk::expression_node *expression;     /* expression nodes */
  cdk::lvalue_node     *lvalue;

  cdk::basic_type      *type;           /* expression type */
  og::block_node       *block;
  std::vector<std::string *> *strings;
};

%token tPUBLIC tREQUIRE tPRIVATE
%token tINT_TYPE tREAL_TYPE tSTRING_TYPE

%token <i> tINTEGER
%token <d> tREAL
%token <s> tIDENTIFIER tSTRING
%token <expression> tAUTO tNULLPTR tPROCEDURE
%token tFOR tWRITE tWRITELN tINPUT tDO tRETURN tBREAK tCONTINUE tPTR
%token tIF tTHEN tELIF tELSE tSIZEOF

%nonassoc tIFX
%nonassoc tTHEN
%nonassoc tELIF tELSE

%right '='
%left tAND tOR
%left tGE tLE tEQ tNE '>' '<'
%left '+' '-'
%left '*' '/' '%'
%nonassoc tUNARY '[' '@'

%type <s> string
%type <node> instr elif
%type <sequence> instrs exprs args inits declarations file innerdecls opt_exprs
%type <expression> expr
%type <lvalue> lval

%type <type> data_type auto void
%type <node> declaration arg init innervar var vardecl fundecl fundef
%type <strings> identifier identifiers
%type <block> block

%{
//-- The rules below will be included in yyparse, the main parsing function.
%}
%%

file            : declarations { compiler->ast($$ = $1); }
                ;

declarations    :              declaration { $$ = new cdk::sequence_node(LINE, $1);     }
                | declarations declaration { $$ = new cdk::sequence_node(LINE, $2, $1); }
                ;

declaration     : vardecl ';' 	{ $$ = $1; }
                | fundecl       { $$ = $1; }
                | fundef        { $$ = $1; }
                ;

arg         :           data_type identifier            	{ $$ = new og::variable_declaration_node(LINE, tPRIVATE,  $1, $2, nullptr); }
            ;

var         : arg                                       	{ $$ = $1; }
            |           data_type identifier  '=' expr  	{ $$ = new og::variable_declaration_node(LINE, tPRIVATE,  $1, $2, $4); }
            |           auto      identifiers '=' exprs 	{ $$ = new og::variable_declaration_node(LINE, tPRIVATE,  $1, $2, new og::tuple_node(LINE, $4)); }
            ;

init		: arg                                       ',' { $$ = $1; }
            |           data_type identifier  '=' expr  ',' { $$ = new og::variable_declaration_node(LINE, tPRIVATE,  $1, $2, $4); }
            |           auto      identifiers '=' exprs ',' { $$ = new og::variable_declaration_node(LINE, tPRIVATE,  $1, $2, new og::tuple_node(LINE, $4)); }
            ;

innervar	: arg                                       ';' { $$ = $1; }
            |           data_type identifier  '=' expr  ';' { $$ = new og::variable_declaration_node(LINE, tPRIVATE,  $1, $2, $4); }
            |           auto      identifiers '=' exprs ';' { $$ = new og::variable_declaration_node(LINE, tPRIVATE,  $1, $2, new og::tuple_node(LINE, $4)); }
            ;

vardecl     : var                                  			{ $$ = $1; }
            | tPUBLIC   data_type identifier            	{ $$ = new og::variable_declaration_node(LINE, tPUBLIC,   $2, $3, nullptr); }
            | tPUBLIC   data_type identifier  '=' expr  	{ $$ = new og::variable_declaration_node(LINE, tPUBLIC,   $2, $3, $5); }
            | tREQUIRE  data_type identifier            	{ $$ = new og::variable_declaration_node(LINE, tREQUIRE,  $2, $3, nullptr); }
            | tREQUIRE  data_type identifier  '=' expr  	{ $$ = new og::variable_declaration_node(LINE, tREQUIRE,  $2, $3, $5); }

            | tPUBLIC   auto      identifiers '=' exprs 	{ $$ = new og::variable_declaration_node(LINE, tPUBLIC,   $2, $3, new og::tuple_node(LINE, $5)); }
            ;

identifiers : identifier                    { $$ = $1; }
            | identifiers ',' tIDENTIFIER   { $$->push_back($3); }
            ;

identifier  : tIDENTIFIER               { $$ = new std::vector<std::string *>(); $$->push_back($1); }
            ;

data_type   : tINT_TYPE                 { $$ = new cdk::primitive_type(4, cdk::typename_type::TYPE_INT); }
            | tREAL_TYPE                { $$ = new cdk::primitive_type(8, cdk::typename_type::TYPE_DOUBLE); }
            | tSTRING_TYPE              { $$ = new cdk::primitive_type(4, cdk::typename_type::TYPE_STRING); }
            | tPTR '<' data_type '>'    { $$ = new cdk::reference_type(4, std::shared_ptr<cdk::basic_type>($3)); }
            | tPTR '<' auto '>'         { $$ = new cdk::reference_type(4, std::shared_ptr<cdk::basic_type>($3)); }
            ;

auto    : tAUTO                         { $$ = new cdk::primitive_type(4, cdk::typename_type::TYPE_UNSPEC); }
        ;

void    : tPROCEDURE                    { $$ = new cdk::primitive_type(4, cdk::typename_type::TYPE_VOID); }
        ;

fundecl :          data_type  tIDENTIFIER '('      ')'       { $$ = new og::function_declaration_node(LINE, tPRIVATE, $1, *$2, nullptr); delete $2; }
        |          data_type  tIDENTIFIER '(' args ')'       { $$ = new og::function_declaration_node(LINE, tPRIVATE, $1, *$2, $4);      delete $2; }
        |          auto       tIDENTIFIER '('      ')'       { $$ = new og::function_declaration_node(LINE, tPRIVATE, $1, *$2, nullptr); delete $2; }
        |          auto       tIDENTIFIER '(' args ')'       { $$ = new og::function_declaration_node(LINE, tPRIVATE, $1, *$2, $4);      delete $2; }
        | tPUBLIC  data_type  tIDENTIFIER '('      ')'       { $$ = new og::function_declaration_node(LINE, tPUBLIC,  $2, *$3, nullptr); delete $3; }
        | tPUBLIC  data_type  tIDENTIFIER '(' args ')'       { $$ = new og::function_declaration_node(LINE, tPUBLIC,  $2, *$3, $5);      delete $3; }
        | tPUBLIC  auto       tIDENTIFIER '('      ')'       { $$ = new og::function_declaration_node(LINE, tPUBLIC,  $2, *$3, nullptr); delete $3; }
        | tPUBLIC  auto       tIDENTIFIER '(' args ')'       { $$ = new og::function_declaration_node(LINE, tPUBLIC,  $2, *$3, $5);      delete $3; }
        | tREQUIRE data_type  tIDENTIFIER '('      ')'       { $$ = new og::function_declaration_node(LINE, tREQUIRE, $2, *$3, nullptr); delete $3; }
        | tREQUIRE data_type  tIDENTIFIER '(' args ')'       { $$ = new og::function_declaration_node(LINE, tREQUIRE, $2, *$3, $5);      delete $3; }
        | tREQUIRE auto       tIDENTIFIER '('      ')'       { $$ = new og::function_declaration_node(LINE, tREQUIRE, $2, *$3, nullptr); delete $3; }
        | tREQUIRE auto       tIDENTIFIER '(' args ')'       { $$ = new og::function_declaration_node(LINE, tREQUIRE, $2, *$3, $5);      delete $3; }

        |          void       tIDENTIFIER '('      ')'       { $$ = new og::function_declaration_node(LINE, tPRIVATE, $1, *$2, nullptr); delete $2; }
        |          void       tIDENTIFIER '(' args ')'       { $$ = new og::function_declaration_node(LINE, tPRIVATE, $1, *$2, $4);      delete $2; }
        | tPUBLIC  void       tIDENTIFIER '('      ')'       { $$ = new og::function_declaration_node(LINE, tPUBLIC,  $2, *$3, nullptr); delete $3; }
        | tPUBLIC  void       tIDENTIFIER '(' args ')'       { $$ = new og::function_declaration_node(LINE, tPUBLIC,  $2, *$3, $5);      delete $3; }
        | tREQUIRE void       tIDENTIFIER '('      ')'       { $$ = new og::function_declaration_node(LINE, tREQUIRE, $2, *$3, nullptr); delete $3; }
        | tREQUIRE void       tIDENTIFIER '(' args ')'       { $$ = new og::function_declaration_node(LINE, tREQUIRE, $2, *$3, $5);      delete $3; }
        ;

fundef  :          data_type  tIDENTIFIER '('      ')' block { $$ = new og::function_definition_node(LINE, tPRIVATE, $1, *$2, nullptr, $5); delete $2; }
        |          data_type  tIDENTIFIER '(' args ')' block { $$ = new og::function_definition_node(LINE, tPRIVATE, $1, *$2, $4,      $6); delete $2; }
        |          auto       tIDENTIFIER '('      ')' block { $$ = new og::function_definition_node(LINE, tPRIVATE, $1, *$2, nullptr, $5); delete $2; }
        |          auto       tIDENTIFIER '(' args ')' block { $$ = new og::function_definition_node(LINE, tPRIVATE, $1, *$2, $4,      $6); delete $2; }
        | tPUBLIC  data_type  tIDENTIFIER '('      ')' block { $$ = new og::function_definition_node(LINE, tPUBLIC,  $2, *$3, nullptr, $6); delete $3; }
        | tPUBLIC  data_type  tIDENTIFIER '(' args ')' block { $$ = new og::function_definition_node(LINE, tPUBLIC,  $2, *$3, $5,      $7); delete $3; }
        | tPUBLIC  auto       tIDENTIFIER '('      ')' block { $$ = new og::function_definition_node(LINE, tPUBLIC,  $2, *$3, nullptr, $6); delete $3; }
        | tPUBLIC  auto       tIDENTIFIER '(' args ')' block { $$ = new og::function_definition_node(LINE, tPUBLIC,  $2, *$3, $5,      $7); delete $3; }

        |          void       tIDENTIFIER '('      ')' block { $$ = new og::function_definition_node(LINE, tPRIVATE, $1, *$2, nullptr, $5); delete $2; }
        |          void       tIDENTIFIER '(' args ')' block { $$ = new og::function_definition_node(LINE, tPRIVATE, $1, *$2, $4,      $6); delete $2; }
        | tPUBLIC  void       tIDENTIFIER '('      ')' block { $$ = new og::function_definition_node(LINE, tPUBLIC,  $2, *$3, nullptr, $6); delete $3; }
        | tPUBLIC  void       tIDENTIFIER '(' args ')' block { $$ = new og::function_definition_node(LINE, tPUBLIC,  $2, *$3, $5,      $7); delete $3; }
        ;

args    : arg           { $$ = new cdk::sequence_node(LINE, $1); }
        | args ',' arg  { $$ = new cdk::sequence_node(LINE, $3, $1); }
        ;

block   : '{'                   '}' { $$ = new og::block_node(LINE, nullptr, nullptr); }
        | '{' innerdecls        '}' { $$ = new og::block_node(LINE, $2, nullptr); }
        | '{'            instrs '}' { $$ = new og::block_node(LINE, nullptr, $2); }
        | '{' innerdecls instrs '}' { $$ = new og::block_node(LINE, $2, $3); }
        ;

innerdecls  :            innervar 	{ $$ = new cdk::sequence_node(LINE, $1); }
            | innerdecls innervar 	{ $$ = new cdk::sequence_node(LINE, $2, $1); }
            ;

inits   : 		innervar    { $$ = new cdk::sequence_node(LINE, $1); }
        | init 	inits  		{ $$ = new cdk::sequence_node(LINE, $1, $2); }
        ;

instrs  : instr	            { $$ = new cdk::sequence_node(LINE, $1); }
        | instrs instr      { $$ = new cdk::sequence_node(LINE, $2, $1); }
        ;

exprs   : expr	            { $$ = new cdk::sequence_node(LINE, $1); }
        | exprs ',' expr    { $$ = new cdk::sequence_node(LINE, $3, $1); }
        ;

opt_exprs   :           { $$ = new cdk::sequence_node(LINE); }
            | exprs     { $$ = $1; }
            ;

instr   : expr ';'                                  { $$ = new og::evaluation_node(LINE, $1); }
        | tWRITE exprs ';'                          { $$ = new og::write_node(LINE, $2, false); }
        | tWRITELN exprs ';'                        { $$ = new og::write_node(LINE, $2, true); }
        | tBREAK                                    { $$ = new og::break_node(LINE);}
        | tCONTINUE                                 { $$ = new og::continue_node(LINE);}
        | tRETURN ';'                               { $$ = new og::return_node(LINE, nullptr); }
        | tRETURN exprs ';'                         { $$ = new og::return_node(LINE, new og::tuple_node(LINE, $2)); }
        | tIF expr tTHEN instr %prec tIFX           { $$ = new og::if_node(LINE, $2, $4); }
        | tIF expr tTHEN instr elif                 { $$ = new og::if_else_node(LINE, $2, $4, $5); }
        | tFOR       ';' opt_exprs ';' opt_exprs tDO instr  { $$ = new og::for_node(LINE, new cdk::sequence_node(LINE), $3, $5, $7); }
        | tFOR inits     opt_exprs ';' opt_exprs tDO instr  { $$ = new og::for_node(LINE, $2, $3, $5, $7); }
        | tFOR exprs ';' opt_exprs ';' opt_exprs tDO instr  { $$ = new og::for_node(LINE, $2, $4, $6, $8); }
        | block                                     { $$ = $1; }
        ;

elif    : tELSE instr                               { $$ = $2; }
        | tELIF expr tTHEN instr                    { $$ = new og::if_node(LINE, $2, $4); }
        | tELIF expr tTHEN instr elif               { $$ = new og::if_else_node(LINE, $2, $4, $5); }
        ;

expr    : tINTEGER              { $$ = new cdk::integer_node(LINE, $1); }
        | tREAL                 { $$ = new cdk::double_node(LINE, $1); }
        | string                { $$ = new cdk::string_node(LINE, $1); }
        | tNULLPTR              { $$ = new og::nullptr_node(LINE); }
        | '-' expr %prec tUNARY { $$ = new cdk::neg_node(LINE, $2); }
        | '+' expr %prec tUNARY { $$ = new og::identity_node(LINE, $2); }
        | '~' expr %prec tUNARY { $$ = new cdk::not_node(LINE, $2); }
        | expr '+' expr	        { $$ = new cdk::add_node(LINE, $1, $3); }
        | expr '-' expr	        { $$ = new cdk::sub_node(LINE, $1, $3); }
        | expr '*' expr	        { $$ = new cdk::mul_node(LINE, $1, $3); }
        | expr '/' expr	        { $$ = new cdk::div_node(LINE, $1, $3); }
        | expr '%' expr	        { $$ = new cdk::mod_node(LINE, $1, $3); }
        | expr '<' expr	        { $$ = new cdk::lt_node(LINE, $1, $3); }
        | expr '>' expr	        { $$ = new cdk::gt_node(LINE, $1, $3); }
        | expr tGE expr	        { $$ = new cdk::ge_node(LINE, $1, $3); }
        | expr tLE expr         { $$ = new cdk::le_node(LINE, $1, $3); }
        | expr tNE expr	        { $$ = new cdk::ne_node(LINE, $1, $3); }
        | expr tEQ expr	        { $$ = new cdk::eq_node(LINE, $1, $3); }
        | expr tAND expr        { $$ = new cdk::and_node(LINE, $1, $3); }
        | expr tOR expr         { $$ = new cdk::or_node(LINE, $1, $3); }
        | '(' expr ')'          { $$ = $2; }
        | '[' expr ']'          { $$ = new og::stack_alloc_node(LINE, $2); }
        | lval                  { $$ = new cdk::rvalue_node(LINE, $1); }
        | lval '=' expr         { $$ = new cdk::assignment_node(LINE, $1, $3); }
        | tINPUT                { $$ = new og::input_node(LINE); }
        | lval '?'              { $$ = new og::address_of_node(LINE, $1); }
        | tSIZEOF '(' exprs ')' { $$ = new og::sizeof_node(LINE, new og::tuple_node(LINE, $3)); }
        | tIDENTIFIER '('       ')' { $$ = new og::function_call_node(LINE, *$1, nullptr); delete $1; }
        | tIDENTIFIER '(' exprs ')' { $$ = new og::function_call_node(LINE, *$1, $3); delete $1; }
        ;

lval    : tIDENTIFIER           { $$ = new cdk::variable_node(LINE, $1); }
        | expr '[' expr ']'     { $$ = new og::index_node(LINE, $1, $3); }
        | expr '@' tINTEGER     { $$ = new og::tuple_index_node(LINE, $1, $3); }
        ;

string  : tSTRING               { $$ = $1; }
        | string tSTRING        { $$ = new std::string(*$1 + *$2); delete $1; delete $2; }
        ;

%%
