/* simple calculator */

%include{
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

//#define DEBUG
}

%token_type <int>
%nonterminal_type <int>

%syntax_error{
    printf("Syntax Error!\n");
    //exit(1);
}

%parse_failed{
    printf("Parse Failed!\n");
    exit(1);
}

%accept{
    printf("Accept!\n");
}

s -> C a D.

a -> A B.

a -> A.

%code{
void main()
{
    void *pParser = ParseAlloc(malloc,realloc);
    ParseTrace(stdout,"label:");
    Parse(pParser,A,5);

    Parse(pParser,0,0);
    ParseFree(pParser,free);
}
}
