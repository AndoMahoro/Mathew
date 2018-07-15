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

start -> s.

s -> c B b a.

a -> a A B.

a -> A B.
b -> c.

b -> d B.

c -> A.

d -> A.

%code{
void main()
{
    void *pParser = ParseAlloc(malloc,realloc);
    ParseTrace(stdout,"label:");
    Parse(pParser,A,5);
    Parse(pParser,B,5);
    Parse(pParser,A,5);
    Parse(pParser,A,5);
    Parse(pParser,B,5);
    Parse(pParser,0,0);
    ParseFree(pParser,free);
}
}
