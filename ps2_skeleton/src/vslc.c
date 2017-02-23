#include <stdio.h>
#include <stdlib.h>
#include <vslc.h>

node_t *root;

int
main ( int argc, char **argv )
{

	//root=(node_t*)malloc(sizeof(node_t));
	//node_init(root, PROGRAM, NULL, ;

	//while(1) { yylex(); }
	root = NULL;
	//yylex();
	yyparse();
	if (root != NULL) {
	    printf("It works\n");
	}
	node_print ( root, 0 );
	destroy_subtree ( root );
}
