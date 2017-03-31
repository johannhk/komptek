#include "vslc.h"
#include <assert.h>


#define MIN(a,b) (((a)<(b)) ? (a):(b))
static const char *record[6] = {
    "%rdi", "%rsi", "%rdx", "%rcx", "%r8", "%r9"
};

#define PRT0(op) puts("\t" #op )
#define PRT1(op, a) puts("\t" #op "\t" #a )
#define PRT2(op, a,b) puts("\t" #op "\t" #a ", " #b )


#define PRTARG1(op, a, ...)    printf("\t" #op "  \t" #a "\n", __VA_ARGS__)
#define PRTARG2(op, a, b, ...) printf("\t" #op "  \t" #a ", " #b "\n", __VA_ARGS__)

#define LABEL(a)    printf(#a ":\n")
#define LABELARG(a, ...)    printf(#a ":\n", __VA_ARGS__)

static size_t if_count=0, while_count=0;
static void
traverse_(symbol_t* func, node_t* root);


static void
generate_stringtable ( void )
{
    /* These can be used to emit numbers, strings and a run-time
     * error msg. from main
     */ 
    puts ( ".section .rodata" );
    puts ( "intout: .string \"\%ld \"" );
    puts ( "strout: .string \"\%s \"" );
    puts ( "errout: .string \"Wrong number of arguments\"" );

    /* TODO:  handle the strings from the program */
    for(int i=0; i<stringc;i++)
    {
        printf("STR%i: .string %s\n",i, string_list[i]);

    }

}
static void
generate_global_names(symbol_t** global_list, size_t n_globals)
{

    puts(".section .data");
    for(size_t i=0;i<n_globals;i++)
    {
        if(global_list[i]->type==SYM_GLOBAL_VAR)
            printf("_%s: .zero 8\n", global_list[i]->name);
    }
}



static void
generate_main ( symbol_t *first )
{
    puts ( ".globl main" );
    puts ( ".section .text" );
    puts ( "main:" );
    puts ( "\tpushq %rbp" );
    puts ( "\tmovq %rsp, %rbp" );

    puts ( "\tsubq $1, %rdi" );
    printf ( "\tcmpq\t$%zu,%%rdi\n", first->nparms );
    puts ( "\tjne ABORT" );
    puts ( "\tcmpq $0, %rdi" );
    puts ( "\tjz SKIP_ARGS" );

    puts ( "\tmovq %rdi, %rcx" );
    printf ( "\taddq $%zu, %%rsi\n", 8*first->nparms );
    puts ( "PARSE_ARGV:" );
    puts ( "\tpushq %rcx" );
    puts ( "\tpushq %rsi" );

    puts ( "\tmovq (%rsi), %rdi" );
    puts ( "\tmovq $0, %rsi" );
    puts ( "\tmovq $10, %rdx" );
    puts ( "\tcall strtol" );

    /*  Now a new argument is an integer in rax */
    puts ( "\tpopq %rsi" );
    puts ( "\tpopq %rcx" );
    puts ( "\tpushq %rax" );
    puts ( "\tsubq $8, %rsi" );
    puts ( "\tloop PARSE_ARGV" );

    /* Now the arguments are in order on stack */
    for ( int arg=0; arg<MIN(6,first->nparms); arg++ )
        printf ( "\tpopq\t%s\n", record[arg] );

    puts ( "SKIP_ARGS:" );
    printf ( "\tcall\t_%s\n", first->name );
    puts ( "\tjmp END" );
    puts ( "ABORT:" );
    puts ( "\tmovq $errout, %rdi" );
    puts ( "\tcall puts" );

    puts ( "END:" );
    puts ( "\tmovq %rax, %rdi" );
    puts ( "\tcall exit" );
    puts ("");
}

char*
get_entry_addr(symbol_t* func, symbol_t* symbol)
{
    static char str[64];
    switch(symbol->type)
    {
        case SYM_GLOBAL_VAR:
            sprintf(str, "_%s", symbol->name);
            break;
        case SYM_PARAMETER:
            sprintf(str, "-%zu(%%rbp)", (symbol->seq+1)*8);
            break;
        case SYM_LOCAL_VAR:
            sprintf(str, "-%zu(%%rbp)", (func->nparms+symbol->seq+1)*8);
            break;
    }
    return str;
}



static void
generate_expression(symbol_t* func, node_t* expr)
{
    char* id_adr;
    
    //function
    if(!expr->data)
    {
    	PRT0(#FUNCTION CALL);
    	node_t* args=expr->children[1];
    	for(int i=0;i<args->n_children;i++)
    	{
    		if(i<=5){
				generate_expression(func, args->children[i]);
				PRTARG2(movq, %%rax, %s, record[i]);	
    		}
    		else {
    			generate_expression(func, args->children[i]);
    			PRT1(pushq, %rax);
    		}
    	}
    	PRTARG1(call, _%s, (char*) expr->children[0]->data);
    	puts("");
    	return;
    }

    else{
	    switch(expr->type)
	    {
	        case IDENTIFIER_DATA:
	            id_adr=get_entry_addr(func, expr->entry);
	            PRTARG2(movq, %s, %%rax, id_adr);
	            break;
	        case NUMBER_DATA:
	            //id_adr=get_entry_addr(func, expr->entry);
	            PRTARG2(movq, $%i, %%rax, *((int*)expr->data));
	            break;
	        case EXPRESSION:
	            
	            if(expr->n_children==2)
	            {
	                //left hand side of expression
	                generate_expression(func, expr->children[0]);
	                //put on stack while getting right side
	                PRT1(pushq, %rax);
	                //right hand side of expression in rax
	                generate_expression(func, expr->children[1]);
	                //pop left hand side to r8
	                PRT1(popq, %r8);
	                
	            }
	            //finds unary and return before switch case
	            else
	            {
	                puts("\t#unary minus");
	                id_adr=get_entry_addr(func, expr->children[0]->entry);
	                PRTARG2(movq, %s, %%rax, id_adr);
	                PRT1(negq, %rax);
	                return;
	            }

	            switch(*(char*)expr->data)
	            {
	                case '+':
	                    puts("\t#addition");
	                    PRT2(addq, %r8, %rax);
	                    break;
	                case '-':
	                    puts("\t#subtraction");
	                    PRT2(xchg, %rax, %r8);
	                    PRT2(subq, %r8, %rax);
	                    break;
	                case '*':
	                    puts("\t#multiplication");
	                    PRT0(cqo);
	                    PRT1(imulq, %r8);
	                    break;
	                case '/':
	                    puts("\t#division");
	                    PRT2(xchg, %rax, %r8);
	                    PRT0(cqo);
	                    PRT1(idivq, %r8);
	                    break;
	                default:
	                    return;

	            }
	    }

	}    
}
static void
generate_assignment(symbol_t* func, node_t* root)
{
    
    
    //generate expression on right hand side of assignment put result in rax
   
    generate_expression(func, root->children[1]);
    //find address of left hand side of assignment
    char* id_adr=get_entry_addr(func, root->children[0]->entry);
    puts("\t#assignment");
    PRTARG2(movq, %%rax, %s, id_adr);
}

static void
generate_print(symbol_t* func, node_t* root)
{
    static char* id_adr;
    for(int i=0;i<root->n_children;i++)
    {
        

        puts("\t#print");
        //PRTARG2(movq, %%rax, %s, record[0]);
        switch(root->children[i]->type)
        {
            case STRING_DATA:
                PRT2(movq, $strout, %rdi);
                PRTARG2(movq, $STR%zu, %%rsi, *((size_t*)root->children[i]->data) );
                break;
            case IDENTIFIER_DATA:
                PRT2(movq, $intout, %rdi);
                id_adr=get_entry_addr(func, root->children[i]->entry);
                PRTARG2(movq, %s, %%rsi, id_adr);
                break;
            case EXPRESSION:
                generate_expression(func, root->children[i]);
                PRT2(movq, $intout, %rdi);
                PRT2(movq, %rax, %rsi);
                break;
            case NUMBER_DATA:
                PRT2(movq, $intout, %rdi);
                PRTARG2(movq, $%ld, %%rsi, *((int64_t*)root->children[i]->data));
                break;
            default:
                assert(0);
        }
        PRT2(movq, $0, %rax);
        PRT0(call printf);
    }
}

static void
generate_return(symbol_t* func, node_t* root)
{
    //move stack pointer up again

    generate_expression(func, root->children[0]);
    PRT0(leave);
    PRT0(ret);
    puts("");
}


static void
generate_function_locals(symbol_t* func)
{
    size_t local_size=tlhash_size(func->locals);
    size_t local_vars=local_size-func->nparms;
    //not sure if neither args and local vars or its just the sum of them that cant be odd
    //print2(subq,)
    PRTARG2(subq, $%i, %%rsp, (int) ((local_size+1)/2)*16);
    for(size_t i=0;i<func->nparms;i++)
    {
        PRTARG2(movq,%s, -%lu(%%rbp),record[i],(i+1)*8);
        if(i>5)
        {
        	PRT1(popq, %rax);
        	PRTARG2(movq, %%rax, -%lu(%%rpb), (i+1)*8);
        }
    }     
    
    //here allocate for local variables
    /*for (size_t i=func->nparms;i<local_size;i++)
    {
    	PRTARG2(movq, %%rax, -%lu(%%rpb), (func->nparms+)*8);
    }*/

    //TODO: IF NECESSARY
}

static void
generate_if_statement(symbol_t* func, node_t* root)
{
	node_t* relation=root->children[0];
	node_t* else_;
	//generate right left side expression
	generate_expression(func, relation->children[0]);
	PRT1(pushq, %rax);
	//generate right hand side of expression
	generate_expression(func, relation->children[1]);
 	PRT1(popq, %r8);
 	PRT0(#IF_CHECK);
 	PRT2(cmpq, %r8, %rax);

 	if(root->n_children==3)
 	{
	 	switch(*(char*)root->children[0]->data)
	 	{
	 		
	 		case '=':
	 			PRTARG1(jne, _ELSE%zu, if_count);
	 			PRTARG1(je, _IF%zu, if_count);
	 			break;
	 		case '<':
	 			PRTARG1(jle, _ELSE%zu, if_count);
	 			PRTARG1(jg, _IF%zu, if_count);
	 			break;

	 		case '>':
	 			PRTARG1(jge, _ELSE%zu, if_count);
	 			PRTARG1(jl, _IF%zu, if_count);
	 			break;
	 	} 
	 	puts("");
 	}

 	
 	LABELARG(_IF%zu, if_count);
 	traverse_(func, root->children[1]);
	PRTARG1(jmp, _ENDIF%zu, if_count); 	
 	

 	LABELARG(_ELSE%zu, if_count);
 	if(root->n_children==3)
 	{
 		traverse_(func, root->children[2]);
 	}
 	PRTARG1(jmp, _ENDIF%zu, if_count);
 	puts("");

 	LABELARG(_ENDIF%zu, if_count);
 	if_count++;
}

static void
generate_while_statement(symbol_t* func, node_t* root)
{

	//while countar than use label

}



static void
traverse_(symbol_t* func, node_t* root)
{

    switch(root->type)
   {
        case ASSIGNMENT_STATEMENT:
            generate_assignment(func, root);
            return;
        case PRINT_STATEMENT:
            generate_print(func, root);
            return;
        case RETURN_STATEMENT:
            generate_return(func, root);
            return;
        case IF_STATEMENT:
        	generate_if_statement(func, root);
        	return;
        case WHILE_STATEMENT:
        	generate_while_statement(func, root);
        	return;

    }

    for(uint64_t i=0;i<root->n_children;i++)
    {
        traverse_(func, root->children[i]);    
    }
}





static void
generate_functions(symbol_t* func){
    
    printf(".globl _%s\n_%s:\n", func->name, func->name);
    PRT1(pushq, %rsp);
    PRT2(movq, %rsp, %rbp);
    generate_function_locals(func);

    traverse_(func, func->node);

    //generate_return(func);
}


 void
generate_program ( void )
{
    size_t n_globals = tlhash_size(global_names);
    symbol_t *global_list[n_globals];
    tlhash_values ( global_names, (void **)&global_list );

    generate_stringtable();
    generate_global_names(global_list, n_globals);
    





    for (size_t i=0;i<n_globals;i++)
    {
        if(global_list[i]->type==SYM_FUNCTION && global_list[i]->seq==0)
        {
            generate_main(global_list[i]);
        }
    }

    for (size_t i=0;i<n_globals;i++)
    {
        if(global_list[i]->type==SYM_FUNCTION)
        {
            generate_functions(global_list[i]);
        }
    }
}
