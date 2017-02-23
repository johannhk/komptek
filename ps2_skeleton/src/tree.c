#include "../include/vslc.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>


void
node_print ( node_t *root, int nesting )
{
    if ( root != NULL )
    {
        /* Print the type of node indented by the nesting level */
        //printf("node has %i children\n",root->n_children);
        printf ( "%*c%s", nesting, ' ', node_string[root->type]);

        /* For identifiers, strings, expressions and numbers,
         * print the data element also
         */
        if ( root->type == IDENTIFIER_DATA ||
             root->type == STRING_DATA ||
             root->type == EXPRESSION ) 
            printf ( "(%s)", (char *) root->data);
        else if ( root->type == NUMBER_DATA )
            printf ( "(%ld)", *((int64_t *)root->data));//http://www.csd.uwo.ca/~moreno/CS447/Lectures/Lexical.html/node11.html
        /* Make a new line, and traverse the node's children in the same manner */
        putchar ( '\n' );
        for ( int64_t i=0; i<root->n_children; i++ )
        {
            //if(root->children[i]==NULL)
            //{
                //printf("%s has child node NULL", node_string[root->type]);}
            node_print ( root->children[i], nesting+1 );
        }
    }
    else {
        printf ( "%*c%p\n", nesting, ' ', root );
	}
}


/* Take the memory allocated to a node and fill it in with the given elements */
void
node_init (node_t *nd, node_index_t type, void *data, uint64_t n_children, ...)
{
	va_list arglist;
	va_start(arglist, n_children);
	nd->type = type;
	nd->data = data;
	nd->n_children=n_children;
	nd->children=(node_t**)malloc(n_children*sizeof(node_t*));
	for (uint64_t i=0;i<n_children;i++)
	{
		nd->children[i]=va_arg(arglist, node_t*);
	}
	va_end(arglist);
}

/* Remove a node and its contents */
void
node_finalize ( node_t *discard )
{
    free(discard->data);
    free(discard->entry);
    free(discard->children);
    free(discard);

}


/* Recursively remove the entire tree rooted at a node */
void
destroy_subtree ( node_t *discard )
{
	for(int i=0;i<discard->n_children;i++)
	{	
		if (discard->children[i]!=NULL)
		{		
			destroy_subtree(discard->children[i]);
		}
	}
	node_finalize(discard);
}
