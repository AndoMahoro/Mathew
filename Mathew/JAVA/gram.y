/* simple calculator */

%include{
package com;

import java.awt.*;

//#define DEBUG
}

%token_type <int>
%nonterminal_type <int>
%left PLUS MINUS.
%left DIVIDE TIMES.
%syntax_error{
    System.out.println("Syntax Error! \n By Shift " + @@ + " value(" + $$ + ")");
}

%parse_failed{
    System.out.println("Parse Failed!");
    System.exit(1);
}

program -> expr($A).{
    System.out.println("Result = " + $A);
}

expr($A) -> expr($B) PLUS expr($C).{
    $A = $B + $C;
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
        System.out.println("Divide by zero!");
}

expr($A) -> INTEGER($B).{$A = $B;}

%code{
    public static void main(String[] args)
    {
        Object pParser = ParseAlloc();
        Parse(pParser,INTEGER,5);
        Parse(pParser,PLUS,0);
        Parse(pParser,INTEGER,6);
        Parse(pParser,TIMES,0);
        Parse(pParser,INTEGER,3);
        Parse(pParser,0,0);
        ParseFree(pParser);
    }
}
