#include <stdio.h>
#include <stdlib.h>
#include "vslc.h"

node_t *root;
node_t **simplified;

int
main ( int argc, char **argv )
{


	root = NULL;

	yyparse();
	if (root != NULL) {
	    printf("It works\n");
	}
	
	//destroy_subtree ( root->child[]);

	//node_print ( root, 0 );
	simplify_tree(root);
	node_print(root,0);

	/
}
