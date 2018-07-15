/* copy right 2017 by mathew c template example */

#line 3 "gram.y"

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#line 10 "yy_table.c"
/* include header */
#include <assert.h>
#include <memory.h>

void TestFunc()
{
    int a;
    unsigned int c;
    printf("Test !");
}

typedef union{
    int yy_11;
}YY_MINOR_TYPE;

#define  PLUS                  1
#define  MINUS                 2
#define  DIVIDE                3
#define  TIMES                 4
#define  INTEGER               5

#define  YY_NSYMBOL 8
#define  YY_NSTATE 11
#define  YY_NRULE 6
#define  YY_ACCEPT_ACTION 17
#define  YY_ERROR_ACTION 18
#define  YY_SHIFT_DEFAULT_OFFSET -6
#define  YY_REDUCE_DEFAULT_OFFSET -6
/* data struct info */
typedef void *(*mallocProc)(size_t);
typedef void *(*reallocProc)(void *, size_t);
typedef void (*freeProc)(void *);

static FILE *yytraceFile = 0;
static char *yytracePrompt = 0;

typedef struct{
    int yymajor;
    int yystate;
    YY_MINOR_TYPE yyminor;
}YY_STACK_ENTRY;

typedef void (*reduceProc)(YY_MINOR_TYPE *, YY_STACK_ENTRY *);

typedef struct{
    int yystack_top;
    int yystack_depth;
    int yyerr_count;
    reallocProc rproc;          /* for stack overflow relloc it */
    YY_STACK_ENTRY yystack[0];
}YY_PARSER;

typedef struct{
    int lhs;
    int nrhs;
}YY_RULE_INFO;


static void yy_accept(YY_PARSER *pParser);

/* static freeProc freeprocs[YY_NSYMBOL + 1];
 * static reduceProc reduceprocs[YY_NRULE]; */

/* action table */
static const unsigned char yy_action[] = {
  10,  17,  1,  11,  2,  4,  8,  6,
  10,  3,  8,  6,  10,  5,  8,  6,
  10,  7,  10,  9,
};
/* verify state table */
static const unsigned char yy_state[] = {
  0,  0,  0,  1,  1,  1,  1,  1,
  2,  2,  3,  3,  4,  4,  5,  5,
  6,  6,  8,  8,
};
/* shift offset table */
static const char yy_shift_offset[] = {
  -5,  3,  3,  7,  7,  11,  11,  -6,
  13,  -6,  -6,
};
/* reduce offset table */
static const char yy_reduce_offset[] = {
  -5,  -6,  2,  -6,  6,  -6,  10,  -6,
  12,  -6,  -6,
};
/* default action table */
static const unsigned char yy_default[] = {
  18,  18,  18,  12,  18,  13,  18,  14,
  18,  15,  16,
};
/* fallback table */
static const unsigned char yy_fallback[] = {
  0, /* # => nothing */
  0, /* PLUS => nothing */
  0, /* MINUS => nothing */
  0, /* DIVIDE => nothing */
  0, /* TIMES => nothing */
  0, /* INTEGER => nothing */
  0, /* program => nothing */
  0, /* expr => nothing */

  0, /* [default] => nothing */

};
/* symbol name */
static const char *yy_symbolName[] = {
  "#",
  "PLUS",
  "MINUS",
  "DIVIDE",
  "TIMES",
  "INTEGER",
  "program",
  "expr",

  "[default]",

};
/* symbol name */
static const char *yy_ruleName[] = {
  "program -> expr.",
  "expr -> expr PLUS expr.",
  "expr -> expr MINUS expr.",
  "expr -> expr TIMES expr.",
  "expr -> expr DIVIDE expr.",
  "expr -> INTEGER.",

};
/* rule info for reduce pop stack */
static const YY_RULE_INFO yyruleInfo[] = {
  {6,1},
  {7,3},
  {7,3},
  {7,3},
  {7,3},
  {7,1},

};
static freeProc freeprocs[9];
static reduceProc reduceprocs[6];

void yy_destructor_0(YY_MINOR_TYPE *yypminor)
{
    /* destructor function */    
}
void yy_destructor_1(YY_MINOR_TYPE *yypminor)
{
    /* destructor function */    
}
void yy_destructor_init()
{
    freeprocs[0] = (freeProc)yy_destructor_0;
    freeprocs[1] = (freeProc)yy_destructor_0;
    freeprocs[2] = (freeProc)yy_destructor_0;
    freeprocs[3] = (freeProc)yy_destructor_0;
    freeprocs[4] = (freeProc)yy_destructor_0;
    freeprocs[5] = (freeProc)yy_destructor_0;
    freeprocs[6] = (freeProc)yy_destructor_1;
    freeprocs[7] = (freeProc)yy_destructor_1;
    freeprocs[8] = (freeProc)yy_destructor_1;
}
void yy_reduce_0(YY_MINOR_TYPE *yypgotominor, YY_STACK_ENTRY *yystack_top)
{
    /* reduce function */    
#line 24 "gram.y"

    printf("Result = %d\n", yystack_top[0].yyminor.yy_11);

#line 179 "yy_table.c"
}
void yy_reduce_1(YY_MINOR_TYPE *yypgotominor, YY_STACK_ENTRY *yystack_top)
{
    /* reduce function */    
#line 28 "gram.y"

    yypgotominor->yy_11 = yystack_top[-2].yyminor.yy_11 + yystack_top[0].yyminor.yy_11;

#line 188 "yy_table.c"
}
void yy_reduce_2(YY_MINOR_TYPE *yypgotominor, YY_STACK_ENTRY *yystack_top)
{
    /* reduce function */    
#line 32 "gram.y"

    yypgotominor->yy_11 = yystack_top[-2].yyminor.yy_11 - yystack_top[0].yyminor.yy_11;

#line 197 "yy_table.c"
}
void yy_reduce_3(YY_MINOR_TYPE *yypgotominor, YY_STACK_ENTRY *yystack_top)
{
    /* reduce function */    
#line 36 "gram.y"

    yypgotominor->yy_11 = yystack_top[-2].yyminor.yy_11 * yystack_top[0].yyminor.yy_11;

#line 206 "yy_table.c"
}
void yy_reduce_4(YY_MINOR_TYPE *yypgotominor, YY_STACK_ENTRY *yystack_top)
{
    /* reduce function */    
#line 40 "gram.y"

    if(yystack_top[0].yyminor.yy_11 != 0)
        yypgotominor->yy_11 = yystack_top[-2].yyminor.yy_11 / yystack_top[0].yyminor.yy_11;
    else
        printf("Divide by zero!\n");

#line 218 "yy_table.c"
}
void yy_reduce_5(YY_MINOR_TYPE *yypgotominor, YY_STACK_ENTRY *yystack_top)
{
    /* reduce function */    
#line 47 "gram.y"
yypgotominor->yy_11 = yystack_top[0].yyminor.yy_11;
#line 225 "yy_table.c"
}
void yy_reduce_init()
{
    reduceprocs[0] = (reduceProc)yy_reduce_0;
    reduceprocs[1] = (reduceProc)yy_reduce_1;
    reduceprocs[2] = (reduceProc)yy_reduce_2;
    reduceprocs[3] = (reduceProc)yy_reduce_3;
    reduceprocs[4] = (reduceProc)yy_reduce_4;
    reduceprocs[5] = (reduceProc)yy_reduce_5;
}

/* inner part */
static const char *get_symbolName(int index)
{
    if(index >= 0 && index < (sizeof(yy_symbolName)/sizeof(yy_symbolName[0]))){
        return yy_symbolName[index];
    }else{
        return "Unknown";
    }
}

static void yy_destructor(int yymajor, YY_MINOR_TYPE *yypminor)
{
    if(freeprocs[yymajor])(*freeprocs[yymajor])(yypminor);
}

static void yy_reduce_proc(int yyrule, YY_MINOR_TYPE *yypgotominor, YY_STACK_ENTRY *yystack_top)
{
    if(reduceprocs[yyrule])(*reduceprocs[yyrule])(yypgotominor,yystack_top);
}

static int yy_pop_stack(YY_PARSER *pParser)
{
    int yymajor;
    YY_STACK_ENTRY *yytop = &pParser->yystack[pParser->yystack_top];

    if(pParser->yystack_top < 0)return 0;

#ifdef DEBUG
    if(yytraceFile){ /* trace stack */
        fprintf(yytraceFile,"%sPoping %s\r\n",yytracePrompt,get_symbolName(yytop->yymajor));
    }
#endif /* DEBUG */

    yymajor = yytop->yymajor;
    yy_destructor(yymajor,&yytop->yyminor); /* free stack top symbol */
    pParser->yystack_top--;
    return yymajor;
}

static int yy_find_shift_action(YY_PARSER *pParser, int lookahead)
{
    int i;
    int state = pParser->yystack[pParser->yystack_top].yystate;

    i = yy_shift_offset[state];

    /* default action */
    if(i == YY_SHIFT_DEFAULT_OFFSET){
        return yy_default[state];
    }

    if(lookahead >= YY_NSYMBOL){
        return YY_ERROR_ACTION;
    }

    i += lookahead;

    if(i < 0 || i >= (sizeof(yy_state)/sizeof(yy_state[0])) || yy_state[i] != state){
        /* try fallback */
        int fallback;
        if(lookahead > 0 && lookahead < (sizeof(yy_fallback)/sizeof(yy_fallback[0])) && (fallback = yy_fallback[0]) != 0){

#ifdef DEBUG
            if(yytraceFile){ /* trace fallback */
                fprintf(yytraceFile,"%sFallback %s => %s\r\n",yytracePrompt,get_symbolName(lookahead),get_symbolName(fallback));
            }
#endif /* DEBUG */

            return yy_find_shift_action(pParser,fallback);
        }
        return yy_default[state]; /* used default action */
    }else{
        return yy_action[i];
    }

}

/* this shift nonterminal is reduce action */
static int yy_find_reduce_action(int state, int lookahead)
{
    int i;

    i = yy_reduce_offset[state];

    if(i == YY_REDUCE_DEFAULT_OFFSET){
        return yy_default[state];
    }

    if(lookahead >= YY_NSYMBOL){
        return YY_ERROR_ACTION;
    }

    i += lookahead;

    if(i < 0 || i >= (sizeof(yy_state)/sizeof(yy_state[0])) || yy_state[i] != state){
        return yy_default[state];
    }else{
        return yy_action[i];
    }

}

static void yy_shift(YY_PARSER **pParser, int yynewState, int yymajor, YY_MINOR_TYPE *yypminor)
{
    YY_STACK_ENTRY *yytop;

    (*pParser)->yystack_top++;

    if((*pParser)->yystack_top >= (*pParser)->yystack_depth){

#ifdef DEBUG
        if(yytraceFile){ /* trace stack overflow */
            fprintf(yytraceFile,"%sStack Overflow ! Try realloc stack.\r\n",yytracePrompt);
        }
#endif /* DEBUG */
        if((*pParser)->rproc && (*pParser = (*((*pParser)->rproc))(*pParser,(size_t)(sizeof(YY_PARSER) + sizeof(YY_STACK_ENTRY) * (*pParser)->yystack_depth * 2))) != 0){
            (*pParser)->yystack_depth *= 2;
        }else{

#ifdef DEBUG
            if(yytraceFile){ /* missing realloc process or realloc stack failure */
                fprintf(yytraceFile,"%sTry realloc stack failure !\r\n",yytracePrompt);
            }
#endif /* DEBUG */
            exit(1);
        }
    }
    /* entry stack */
    yytop = &(*pParser)->yystack[(*pParser)->yystack_top];
    yytop->yystate = yynewState;
    yytop->yymajor = yymajor;
    yytop->yyminor = *yypminor;

#ifdef DEBUG
    if(yytraceFile && (*pParser)->yystack_top > 0){
        int i;
        fprintf(yytraceFile,"%sShift %s => State(%d)\r\n",yytracePrompt,get_symbolName(yymajor),yynewState);
        fprintf(yytraceFile,"%sStack:",yytracePrompt);
        for(i = 0; i <= (*pParser)->yystack_top; i++)fprintf(yytraceFile," %s",get_symbolName((*pParser)->yystack[i].yymajor));
        fprintf(yytraceFile,"\r\n");
    }
#endif /* DEBUG */

}

static void yy_reduce(YY_PARSER *pParser, int yyrule)
{
    int yygoto;
    int yyaction;
    int yysize;

    YY_MINOR_TYPE yygotominor;
    YY_STACK_ENTRY *yytop;

    yytop = &pParser->yystack[pParser->yystack_top];

#ifdef DEBUG
        if(yytraceFile && yyrule >= 0 && yyrule < (sizeof(yy_ruleName)/sizeof(yy_ruleName[0]))){ /* missing realloc process or realloc stack failure */
            fprintf(yytraceFile,"%sReduce [%s]\r\n",yytracePrompt,yy_ruleName[yyrule]);
        }
#endif /* DEBUG */
    memset((void*)&yygotominor,0,sizeof(yygotominor));

    yy_reduce_proc(yyrule,&yygotominor,yytop); /* invoke reduce code */

    yygoto = yyruleInfo[yyrule].lhs;
    yysize = yyruleInfo[yyrule].nrhs;
    pParser->yystack_top -= yysize;
    yyaction = yy_find_reduce_action(yytop[-yysize].yystate,yygoto);

    if(yyaction < YY_NSTATE){/* nonterminal shift so yyaction need less state number */

#ifndef DEBUG
       /* if not debugging direct shift action improve performance */
       if(1)
       {
           pParser->yystack_top++;
           yytop = &pParser->yystack[pParser->yystack_top];
           yytop->yystate = yyaction;
           yytop->yymajor = yygoto;
           yytop->yyminor = yygotominor;
       }
       else
#endif /* DEBUG */
       {
           yy_shift(&pParser,yyaction,yygoto,&yygotominor);
       }
    }else if(yyaction == YY_ACCEPT_ACTION){
        yy_accept(pParser);
    }
}

/* syntax error */
static void yy_syntax_error(YY_PARSER *pParser, int yymajor, YY_MINOR_TYPE *yypminor)
{

#line 14 "gram.y"

    printf("Syntax Error!\n By Shift %d value(%d)\n",yymajor,yypminor->yy_11);
    //exit(1);

#line 438 "yy_table.c"
}

/* parse failed */
static void yy_parse_failed(YY_PARSER *pParser)
{

#ifdef DEBUG
    if(yytraceFile){
        fprintf(yytraceFile,"%sParse Failed!\r\n",yytracePrompt);
    }
#endif

#line 19 "gram.y"

    printf("Parse Failed!\n");
    exit(1);
#line 455 "yy_table.c"
}

/* accept process */
static void yy_accept(YY_PARSER *pParser)
{

#ifdef DEBUG
    if(yytraceFile){
        fprintf(yytraceFile,"%sAccept!\r\n",yytracePrompt);
    }
#endif

    while(pParser->yystack_top >= 0)yy_pop_stack(pParser);
}

/* interface part */
/* Initialize trace configuration */
void ParseTrace(FILE *traceFile, char *tracePrompt)
{
    yytraceFile = traceFile;
    yytracePrompt = tracePrompt;
    if(yytracePrompt == 0){
        yytracePrompt = "";
    }
}

void * ParseAlloc(mallocProc mproc, reallocProc rproc)
{
    YY_PARSER *pParser;
    int yystack_depth = 100;

    /* init process */
    yy_destructor_init();
    yy_reduce_init();

    pParser = (YY_PARSER*)(*mproc)((size_t)(sizeof(YY_PARSER) + sizeof(YY_STACK_ENTRY) * yystack_depth));

    if(pParser){
        pParser->yystack_top = -1;
        pParser->yystack_depth = yystack_depth;
        pParser->yyerr_count = 0;
        pParser->rproc = rproc;
    }

    return pParser;
}

void ParseFree(void *p, freeProc fproc)
{
    YY_PARSER *pParser = (YY_PARSER *)p;
    if(pParser == 0)return;
    while(pParser->yystack_top >= 0)yy_pop_stack(pParser);
    if(fproc)(*fproc)(pParser);
}

void Parse(void *yypParser, int yymajor, int yyminor)
{
    YY_MINOR_TYPE yyminorunion;
    YY_PARSER *pParser;
    int yyendofinput;
    int yyaction;

    pParser = (YY_PARSER *)yypParser;

    /* initialize */
    if(pParser->yystack_top < 0){
        pParser->yystack_top = 0;
        pParser->yyerr_count = -1;
        pParser->yystack[0].yymajor = 0; /* '#' */
        pParser->yystack[0].yystate = 0; /* state number */

    }
    /* yyminorunion.yy_%d = yyminor; */
    yyminorunion.yy_11 = yyminor;
    yyendofinput = (yymajor == 0);

#ifdef DEBUG
    if(yytraceFile){
        fprintf(yytraceFile,"%sInput %s\r\n",yytracePrompt,get_symbolName(yymajor));
    }
#endif

    do{
        yyaction = yy_find_shift_action(pParser,yymajor);

        if(yyaction < YY_NSTATE){
            yy_shift(&pParser,yyaction,yymajor,&yyminorunion);
            pParser->yyerr_count--;
            if(yyendofinput && pParser->yystack_top >= 0){
                yymajor = 0;
            }else{
                yymajor = YY_NSYMBOL; /* end of loop flag for shift after reduce action */
            }
        }else if(yyaction < YY_NSTATE + YY_NRULE){
            yy_reduce(pParser,yyaction - YY_NSTATE); /* shift nt for next loop yy_find_shift_action find lookahead(yymajor) reduce rule */
        }else if(yyaction == YY_ERROR_ACTION){

#ifdef DEBUG
            if(yytraceFile){
                fprintf(yytraceFile,"%sSyntax Error!\r\n",yytracePrompt);
            }
#endif

            if(pParser->yyerr_count <= 0){
                yy_syntax_error(pParser,yymajor,&yyminorunion);
            }
            pParser->yyerr_count = 3; /* 3 times error only show 1 time */
            yy_destructor(yymajor,&yyminorunion); /* drop the cause of the error token(yymajor) */

            if(yyendofinput){
                yy_parse_failed(pParser);
            }
            yymajor = YY_NSYMBOL;

        }else{
            yy_accept(pParser);
            yymajor = YY_NSYMBOL;
        }

    }while(yymajor != YY_NSYMBOL && pParser->yystack_top >= 0);

}
#line 49 "gram.y"

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
#line 592 "yy_table.c"

/* after extra code */

