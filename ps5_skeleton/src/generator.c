#include "vslc.h"

#define MIN(a,b) (((a)<(b)) ? (a):(b))
static const char *record[6] = {
    "%rdi", "%rsi", "%rdx", "%rcx", "%r8", "%r9"
};

#define PRT0(op) puts("\t" #op )
#define PRT1(op, a) puts("\t" #op "\t" #a )
#define PRT2(op, a,b) puts("\t" #op "\t" #a ", " #b )


#define PRTARG1(op, a, ...)    printf("\t" #op "  \t" #a "\n", __VA_ARGS__)
#define PRTARG2(op, a, b, ...) printf("\t" #op "  \t" #a ", " #b "\n", __VA_ARGS__)




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

    //for 
    /* TODO:  handle the strings from the program */
    for(int i=0; i<stringc;i++)
    {
        printf("STR%i:\n\t.%s\n",i, string_list[i]);

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
        case SYM_LOCAL_VAR:
            sprintf(str, "-%zu(%%rbp)", (func->nparms+symbol->seq+1)*8);
    }
    return str;
}



static void
generate_expression(symbol_t* func, node_t* expr)
{
    char* id_adr;
    char* reg_adr="%%r8";
    for(uint64_t i=0;i<expr->n_children;i++)
    {
        //if(i==0)
        //    reg_adr="%rax"
        switch(expr->->type)
        {

            case IDENTIFIER_DATA:
                printf("ident\n");
                id_adr=get_entry_addr(func, expr->entry);
                PRTARG2(movq, %s, %%rax, id_adr);
                break;
            case NUMBER_DATA:
                //id_adr=get_entry_addr(func, expr->entry);
                PRTARG2(movq, %i, %%rax, *((int*)expr->data));
                break;
            case EXPRESSION:
                printf("expr %s\n",expr->data);
                generate_expression(func, expr->children[i]);
                break;
        }
    }
}

void
traverse_(symbol_t* func, node_t* root)
{
    for(uint64_t i=0;i<root->n_children;i++)
    {
        //if(root->children[i]->type==IDENTIFIER_DATA || root->children[i]->type==NUMBER_DATA )
        if(root->type==EXPRESSION)
            generate_expression(func, root);
        else
            traverse_(func, root->children[i]);
    }

    return;
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
        PRTARG2(movq,%s, -%lu(%%rpb),record[i],(i+1)*8);
    }     
    
    //here do somehting for local variables
    for (size_t i=0;i<local_size;i++)
    {
        //if(func->locals[i]->type=SYM_LOCAL_VAR)
        //    printf("\tmovq");
    }

    //TODO: IF NECESSARY
}

static void
generate_functions(symbol_t* func){
    size_t local_size=tlhash_size(func->locals);
    printf(".globl _%s\n_%s:\n", func->name, func->name);
    PRT1(pushq, %rsp);
    PRT2(movq, %rsp, %rbp);
    generate_function_locals(func);
    traverse_(func, func->node);

    
    //return by putting in rax
    //printf()
    printf("\taddq $%i, %%rsp\n",(int) ((local_size+1)/2)*16);
}


void
generate_program ( void )
{
    size_t n_globals = tlhash_size(global_names);
    symbol_t *global_list[n_globals];
    tlhash_values ( global_names, (void **)&global_list );


    generate_stringtable();
    generate_global_names(global_list, n_globals);
    generate_main(global_list[0]);

    for (size_t i=0;i<n_globals;i++)
    {
        if(global_list[i]->type==SYM_FUNCTION)
            generate_functions(global_list[i]);
    }

    /* Put some dummy stuff to keep the skeleton from crashing */

    puts ( "\tmovq $0, %rax" );
    puts ( "\tcall exit" );
}
