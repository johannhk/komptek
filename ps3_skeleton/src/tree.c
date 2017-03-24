#include <stdarg.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <assert.h>
#include "vslc.h"
#define SINGLE_CHILD(child) ((child)->type==GLOBAL || (child)->type==STATEMENT || \
                (child)->type==PRINT_ITEM || (child)->type==PARAMETER_LIST || (child)->type==ARGUMENT_LIST)

#define LIST_CHILD(child) ((child)->type==GLOBAL_LIST || (child)->type==STATEMENT_LIST || (child)->type==PRINT_LIST || \
				(child)->type==EXPRESSION_LIST || (child)->type==VARIABLE_LIST)
#define ARIT_CHILD(child) ( *(char*)(child)->data=='*' || *(char*)(child)->data=='/' || *(char*)(child)->data=='+' ||\
                 *(char*)(child)->data=='-')
#define EXPR_CHILD(child) ( (child)->type==  || (child)->type==GLOBAL_LIST  )


void
node_print ( node_t *root, int nesting )
{
    if ( root != NULL )
    {
        /* Print the type of node indented by the nesting level */
        //printf("node has %i children\n",root->n_children);
        printf ( "%*c%s with %lu children", nesting, ' ', node_string[root->type], root->n_children);

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
    assert(discard != NULL);

    free(discard->data);
    free(discard->entry);
    free(discard->children);
    discard->data = NULL;
    discard->entry = NULL;
    discard->children = NULL;

}
/* Recursively remove the entire tree rooted at a node */
void
destroy_subtree ( node_t *discard )
{
	for(uint64_t i=0;i<discard->n_children;i++)
	{
		//check to ignore child nodes that are empty (empty lists)
		if(!discard->children[i])
			continue;
		destroy_subtree(discard->children[i]);
	}
	node_finalize(discard);
}



void 
simplify_single_node(node_t *parent, node_t** child, int i)
{
	//printf("called single on %s and %i children\n",node_string[parent->type], parent->n_children);

	parent->children[i]=(*child)->children[0];
	node_finalize(*child);
	*child=parent->children[i];
}

void
simplify_list_rec(node_t* parent, node_t*** new_children, int depth)
{
    node_t* next=NULL;
    for (uint64_t i=0; i<parent->n_children; i++)   
    {
        //checking is there exists a child of type list
        if (LIST_CHILD(parent->children[i]))
        {
            next=parent->children[i];
        }
    }
    //if child exists continue deeper
    if (next)
    {
        simplify_list_rec(next, new_children, depth + 1);
        (*new_children)[depth] = parent->children[1];
    }
    else
    {
        *new_children = malloc((depth + 1) * sizeof(node_t *));
        (*new_children)[depth] = parent->children[0];
    }
    if (depth > 0) {
        node_finalize(parent);
    }
}


void
simplify_list(node_t *parent)
{
    if(parent->n_children>1)
    {
        node_t** new_children;
        simplify_list_rec(parent, &new_children, 0);
        free(parent->children);
        parent->children = new_children;
    }
    
}


void
simplify_exp(node_t* parent)
{
	if (parent->n_children>1) 
    {
        for (uint64_t i=0; i<parent->n_children; i++)
	    {

            node_t* child=parent->children[i];
            if(EXPR_CHILD)








            switch(*(char*)child->data)
            {
                case '*':
                    break;
                case '/':
                    break;
                case '-':
                    if(child)
                    break;
                case '+':
                    break;
                default:
                    break;
            }

        }

    }

    else
    {
        
    }

}



void
simplify_tree (node_t *root )
{

    for (uint64_t i=0;i<root->n_children; i++)
    {
    	node_t* child=root->children[i];
    	if(!child)
    	{
    		printf("child of node %s is zero\n", node_string[root->type]);
    		continue;
    	}
        if (SINGLE_CHILD(child)) {
	       	simplify_single_node(root, &child, i);
        }
       	if (LIST_CHILD(child)) {
       		printf("starting list routine\n");
           	simplify_list(child);
        }
        if (ARIT_CHILD(child)) {
        	//simplify_arithmetic(child);
        }
        simplify_tree(child);

    }
}

