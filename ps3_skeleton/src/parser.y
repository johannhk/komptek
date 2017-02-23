%{
#include "../include/vslc.h"
#include <stdio.h>
%}
%left '+' '-'
%left '*' '/'
%nonassoc UMINUS

%token FUNC PRINT RETURN CONTINUE IF THEN ELSE WHILE DO OPENBLOCK CLOSEBLOCK
%token VAR NUMBER IDENTIFIER STRING
/*extern int  yylineno;
extern char* yytext[];
extern FILE* outFile_p;
int noerror=1;
*/
//cat vsl_programs/euclid.vsl | src/vslc 
%%
program 
	: global_list 	{
			$$ = malloc ( sizeof(node_t) );
			node_init($$, PROGRAM, 0, 1, $1);
			//printf("root node init\n");
			root=$$;}
	;


global_list 	
    : global {
		$$ = (node_t *) malloc ( sizeof(node_t) );
		node_init($$, GLOBAL_LIST, 0, 1, $1); }
	| global_list global {
		$$ = (node_t *) malloc ( sizeof(node_t) );
		node_init($$, GLOBAL_LIST, 0, 2, $1, $2);
		root=$$; }
	;

global		
    : function { 
        $$ = (node_t *) malloc ( sizeof(node_t) );
		node_init($$, GLOBAL, 0, 1, $1); }
	| declaration { 
	    $$ = (node_t *) malloc ( sizeof(node_t) );
		node_init($$, GLOBAL, 0, 1, $1); }
	;
statement_list	
    : statement { 
        $$ = (node_t *) malloc ( sizeof(node_t) );
		node_init($$, STATEMENT_LIST, 0, 1, $1); }

	| statement_list statement { $$ = (node_t *) malloc ( sizeof(node_t) );
		node_init($$, STATEMENT_LIST, 0, 2, $1, $2); }

	;
print_list	
    : print_item { 
        $$ = (node_t *) malloc ( sizeof(node_t) );
		node_init($$, PRINT_LIST, 0, 1, $1); }

	| print_list ',' print_item { 
	    $$ = (node_t *) malloc ( sizeof(node_t) );
		node_init($$, PRINT_LIST, strdup(","), 2, $1, $3); }
	;
expression_list	
    : expression { 
        $$ = (node_t *) malloc ( sizeof(node_t) );
		node_init($$, EXPRESSION_LIST, 0, 1, $1); }
	| expression_list ',' expression {
	     $$ = (node_t *) malloc ( sizeof(node_t) );
		node_init($$, EXPRESSION_LIST, strdup(","), 2, $1, $3); }
		;
variable_list	
    : identifier { 
        $$ = (node_t *) malloc ( sizeof(node_t) );
        node_init($$, VARIABLE_LIST, 0, 1, $1); }

	| variable_list ',' identifier { $$ = (node_t *) malloc ( sizeof(node_t) );
		node_init($$, VARIABLE_LIST, 0, 2, $1, $3); }
		;
argument_list 	
    : expression_list { 
        $$ = (node_t *) malloc ( sizeof(node_t) );
		node_init($$, ARGUMENT_LIST, 0, 1, $1); }
	| {$$ = NULL;}
	;
parameter_list	
    : variable_list { $$ = (node_t *) malloc ( sizeof(node_t) );
    	node_init($$, PARAMETER_LIST, 0, 1, $1); }
	| {$$ = NULL;}	
	;
declaration_list
    : declaration { 
        $$ = (node_t *) malloc ( sizeof(node_t) );
        node_init($$, DECLARATION_LIST, 0, 1, $1); }
	| declaration_list declaration { 
	    $$ = (node_t *) malloc ( sizeof(node_t) );
		node_init($$, DECLARATION_LIST , 0, 2, $1, $2); }
	;
function	
    : FUNC identifier '(' parameter_list ')' statement { 
        $$ = (node_t *) malloc ( sizeof(node_t) );
		node_init($$, FUNCTION , 0, 3, $2, $4, $6);
		//node_print($$,0); 
		}
	;

statement	
    : assignment_statement { 
        $$ = (node_t *) malloc ( sizeof(node_t) );
		node_init($$, STATEMENT, 0, 1, $1); }
	| return_statement { 
	    $$ = (node_t *) malloc ( sizeof(node_t) );
		node_init($$, STATEMENT, 0, 1, $1); }
	| print_statement { 
	    $$ = (node_t *) malloc ( sizeof(node_t) );
		node_init($$, STATEMENT, 0, 1, $1); }
	| if_statement {
	    $$ = (node_t *) malloc ( sizeof(node_t) );
		node_init($$, STATEMENT, 0, 1, $1); }
	| while_statement { 
	    $$ = (node_t *) malloc ( sizeof(node_t) );
		node_init($$, STATEMENT, 0, 1, $1); }
	| null_statement { 
	    $$ = (node_t *) malloc ( sizeof(node_t) );
		node_init($$, STATEMENT, 0, 1, $1); }
	| block { 
	    $$ = (node_t *) malloc ( sizeof(node_t) );
		node_init($$, STATEMENT, 0, 1, $1); }
	;
block		
    : OPENBLOCK declaration_list statement_list CLOSEBLOCK { 
        $$ = (node_t *) malloc ( sizeof(node_t) );
		node_init($$, BLOCK, 0, 2, $2, $3); }
	| OPENBLOCK statement_list CLOSEBLOCK {
	    $$ = (node_t *) malloc ( sizeof(node_t) );
		node_init($$, BLOCK, 0, 1, $2); }
	;
assignment_statement 
    : identifier ':' '='  expression { 
        $$ = (node_t *) malloc ( sizeof(node_t) );
		node_init($$, ASSIGNMENT_STATEMENT, strdup(":="), 2, $1, $4); }
	;
return_statement 
    : RETURN expression { 
        $$ = (node_t *) malloc ( sizeof(node_t) );
		node_init($$, RETURN_STATEMENT, 0, 1, $2); }
	;
print_statement 
    : PRINT print_list { 
        $$ = (node_t *) malloc ( sizeof(node_t) );
		node_init($$, PRINT_STATEMENT, 0, 1, $2); }
	;
null_statement	
    : CONTINUE { 
        $$ = (node_t *) malloc ( sizeof(node_t) );
		node_init($$, NULL_STATEMENT, 0, 0); }
	;
if_statement	
    : IF relation THEN statement { 
        $$ = (node_t *) malloc ( sizeof(node_t) );
		node_init($$, IF_STATEMENT, 0, 2, $2, $4); }
	| IF relation THEN statement ELSE statement { 
	    $$ = (node_t *) malloc ( sizeof(node_t) );
		node_init($$, IF_STATEMENT, 0, 3, $2, $4, $6); }
	;
while_statement 
    : WHILE relation DO statement { 
        $$ = (node_t *) malloc ( sizeof(node_t) );
		node_init($$, WHILE_STATEMENT, 0, 2, $2, $4); }
	;
relation 	
    : expression '=' expression { 
        $$ = (node_t *) malloc ( sizeof(node_t) );
		node_init($$, RELATION, 0, 2, $1, $3); }
	| expression '<' expression { 
	    $$ = (node_t *) malloc ( sizeof(node_t) );
		node_init($$, RELATION, 0, 2, $1, $3); }
	| expression '>' expression { 
	    $$ = (node_t *) malloc ( sizeof(node_t) );
		node_init($$, RELATION, 0, 2, $1, $3); }
	;
expression	
    : expression '+' expression { 
        $$ = (node_t *) malloc ( sizeof(node_t) );
		node_init($$, EXPRESSION, strdup("+"), 2, $1, $3); } 
	| expression '-' expression { 
	    $$ = (node_t *) malloc ( sizeof(node_t) );
		node_init($$, EXPRESSION, strdup("-"), 2, $1, $3); } 
	| expression '*' expression { 
	    $$ = (node_t *) malloc ( sizeof(node_t) );
		node_init($$, EXPRESSION, strdup("*"), 2, $1, $3); } 
	| expression '/' expression { 
	    $$ = (node_t *) malloc ( sizeof(node_t) );
		node_init($$, EXPRESSION, strdup("/"), 2, $1, $3); } 
	| '-' expression %prec UMINUS { 
	    $$ = (node_t *) malloc ( sizeof(node_t) );
		node_init($$, EXPRESSION, strdup("-"), 1, $2); } 
	| '(' expression ')' { 
	    $$ = $2; } 
	| number { $$ = (node_t *) malloc ( sizeof(node_t) );   
		node_init($$, EXPRESSION, 0, 1, $1); } 
	| identifier { 
	    $$ = (node_t *) malloc ( sizeof(node_t) );
		node_init($$, EXPRESSION, 0, 1, $1); } 
	| identifier '(' argument_list ')' { 
	    $$ = (node_t *) malloc ( sizeof(node_t) );
		node_init($$, EXPRESSION, 0, 2, $1, $3); } 
	;
		
declaration	: VAR variable_list {
        $$ = (node_t *) malloc ( sizeof(node_t) );
		node_init($$, DECLARATION, 0, 1, $2); } 
		;
print_item	
    : expression { $$ = (node_t *) malloc ( sizeof(node_t) );
		node_init($$, PRINT_ITEM, 0, 1, $1); }
	| string { $$ = (node_t *) malloc ( sizeof(node_t) );
		node_init($$, PRINT_ITEM, 0, 1, $1); }
	;
identifier
	: IDENTIFIER {
		$$=(node_t *) malloc ( sizeof(node_t) );
		char* buf=strdup(yytext);
		node_init($$,IDENTIFIER_DATA, buf, 0);
	}
	;

string 	
	: STRING
	{
		$$=(node_t *) malloc ( sizeof(node_t) );
		char* buf=strdup(yytext);
		node_init($$,STRING_DATA, buf, 0);			
	}
	;

number		
    : NUMBER 	{ 
	        $$ = malloc (sizeof(node_t));
			int64_t* buf=(int64_t*) malloc(sizeof(int64_t));
			*buf=strtol(yytext,NULL, 10);
			node_init($$,NUMBER_DATA, buf, 0);	
	}
	;

%%



int
yyerror ( const char *error )
{
    fprintf ( stderr, "%s on line %d\n", error, yylineno );
    exit ( EXIT_FAILURE );
}

