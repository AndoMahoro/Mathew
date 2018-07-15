/* simple calculator */

%include{
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

}

%token_type <int>
%nonterminal_type <int>
%left PLUS MINUS.
%left DIVIDE TIMES.
%syntax_error{
    printf("Syntax Error!\n By Shift %d value(%d)\n",@@,$$);
    //exit(1);
}

%parse_failed{
    printf("Parse Failed!\n");
    exit(1);
}

program -> expr ($A).{
    printf("Result = %d\n", $A);
}

expr($A) -> expr($B) PLUS expr($C).{
    $A = $B + $3;
}

expr($A) -> expr($B) MINUS expr($C).{
    $A = $B - $C;
}

expr($A) -> expr($B) TIMES expr($C).{
    $A = $B * $C;
}

expr($A) -> expr($B) DIVIDE expr($C).{
    if($C != 0)
        $A = $B / $C;
    else
        printf("Divide by zero!\n");
}

expr($A) -> INTEGER($B).{$A = $B;}

%code{
void main()
{
    void *pParser = ParseAlloc(malloc,realloc);
    ParseTrace(stdout,"label:");
    Parse(pParser,INTEGER,5);
    Parse(pParser,PLUS,0);
    Parse(pParser,INTEGER,6);
    Parse(pParser,TIMES,0);
    Parse(pParser,INTEGER,3);
    Parse(pParser,0,0);
    ParseFree(pParser,free);
}
}
