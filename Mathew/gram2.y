/* Copyright (GPL) 2017 by mps email: rk1135270948@163.com
** Simple mathew parser  example. %ifdef Win32 %endif
** Test grammar file */


%include {   
#include <iostream>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h> 
}  

%token_type <int/* >*/>
%type <node* > nt_1 nt_2 nt_3.
%type <int > PULS.
%nonterminal_type <expr*>

%destructor {free($$);} nt_1 nt_2.
%default_code {cout<<"default reduce code"<<endl;}
%left PLUS MINUS .
%left DIVIDE TIMES . 

%syntax_error {
   std::cout << " Syntax error! "  << std::endl; //%ifdef Win32 %endif

}

%start progam

/*program -> expr($A).   { %ifdef Win32 std::cout << %ifndef Win32 "Result=" <<%endif $A << std::endl; %endif }*/

expr($A) -> expr($B) MINUS {std::cout<< " - " <<std::endl;}($E) $expr($C).   { $A = $B - $C; }
expr($A) -> expr($B) PLUS   expr($C).   { $A = $B + $C; }
expr($A) -> cc expr($B) TIMES  expr($C).   { $A = $B * $C; }
expr($A) ->  dd expr($B) {std::cout<< " Pre div " <<std::endl;} DIVIDE kd expr($C).   {

         if($C != 0){
           $A = $B / $C;
         }else{
           std::cout << "divide by zero" << std::endl;
         }
}  /* end of DIVIDE */

expr($A) -> INTEGER($B). { $A = $B; }

cc -> ll kk mm.
kk->.
kk-> TK_YES.
ll-> TK_NO.
ll->.
switch($b)-> case .
dsdf->.

%code{
  void main()
  {
     void* pParser = ParseAlloc (malloc);
     Parse (pParser, INTEGER, 1);
     Parse (pParser, PLUS, 0);
     Parse (pParser, INTEGER, 2);
     Parse (pParser, 0, 0);
     ParseFree(pParser, free );
  }
}

