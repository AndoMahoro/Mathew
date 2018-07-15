%out                    <yy_table.c>                                                
%line_start             <1>
%special_escape         < >
%line                   <#line %d "%s"\r\n>                                         
%include                <#include "%s"\r\n>                                         
%include_file           <yy_table.h>                                                
%token                  <#define  %s%-20s %2d\r\n>                                  
%parse_name             <Parse>
%parse_decl             <\r\nvoid %s(void *yypParser, int yymajor, %s yyminor);\r\n>
%parse_func_prefix      <void %s(void *yypParser, int yymajor, %s yyminor)\r\n{\r\n>
%parse_func_minor       <    yyminorunion.yy_%d = yyminor;\r\n>
%parse_func_suffix      <}\r\n>
%type                   <unsigned char,unsigned short,unsigned int,unsigned long,char,short,int,long>  /* java: <Object,Object> */
%symbol_num             <#define  YY_NSYMBOL %d\r\n>
%state_num              <#define  YY_NSTATE %d\r\n>
%rule_num               <#define  YY_NRULE %d\r\n>
%accept_action          <#define  YY_ACCEPT_ACTION %d\r\n>
%error_action           <#define  YY_ERROR_ACTION %d\r\n>
%shift_default_offset   <#define  YY_SHIFT_DEFAULT_OFFSET %d\r\n>
%reduce_default_offset  <#define  YY_REDUCE_DEFAULT_OFFSET %d\r\n>
%minor_prefix           <\r\ntypedef union{\r\n>
%minor_tok_type         <int>
%minor_nt_type          <void *>
%minor_member           <    %s yy_%d;\r\n>
%minor_suffix           <}YY_MINOR_TYPE;\r\n\r\n>
%array_width            <8>
%action_prefix          </* action table */\r\nstatic const %s yy_action[] = {\r\n>
%action_member          <  %d,>
%action_suffix          <};\r\n>
%state_prefix           </* verify state table */\r\nstatic const %s yy_state[] = {\r\n>
%state_member           <  %d,>
%state_suffix           <};\r\n>
%shift_offset_prefix    </* shift offset table */\r\nstatic const %s yy_shift_offset[] = {\r\n>
%shift_offset_member    <  %d,>
%shift_offset_suffix    <};\r\n>
%reduce_offset_prefix   </* reduce offset table */\r\nstatic const %s yy_reduce_offset[] = {\r\n>
%reduce_offset_member   <  %d,>
%reduce_offset_suffix   <};\r\n>
%default_prefix         </* default action table */\r\nstatic const %s yy_default[] = {\r\n>
%default_member         <  %d,>
%default_suffix         <};\r\n>
%fallback_prefix        </* fallback table */\r\nstatic const %s yy_fallback[] = {\r\n>
%fallback_empty         <  0, /* %s =!> nothing */\r\n>
%fallback_member        <  %d, /* %s =!> %s */\r\n>
%fallback_suffix        <};\r\n>
%symbol_prefix          </* symbol name */\r\nstatic const char *yy_symbolName[] = {\r\n>
%symbol_member          <  "%s",\r\n>
%symbol_suffix          <};\r\n>
%rule_prefix            </* symbol name */\r\nstatic const char *yy_ruleName[] = {\r\n>
%rule_member_prefix     <  ">
%rule_member_suffix     <",\r\n>
%rule_suffix            <};\r\n>
%rule_info_prefix       </* rule info for reduce pop stack */\r\nstatic const YY_RULE_INFO yyruleInfo[] = {\r\n>
%rule_info_member       <  {%d,%d},\r\n>
%rule_info_suffix       <};\r\n>
%destructor_prefix      <void yy_destructor_%d(YY_MINOR_TYPE *yypminor)\r\n{\r\n\
    /* destructor function */    \r\n>
%destructor_arg         <yypminor-!>yy_%d>
%destructor_suffix      <}\r\n>
%destructor_init_prefix <void yy_destructor_init()\r\n{\r\n>
%destructor_init_member <    freeprocs[%d] = (freeProc)yy_destructor_%d;\r\n>
%destructor_init_suffix <}\r\n>
%reduce_prefix          <void yy_reduce_%d(YY_MINOR_TYPE *yypgotominor, YY_STACK_ENTRY *yystack_top)\r\n{\r\n\
    /* reduce function */    \r\n>
%reduce_arg_lvalue      <yypgotominor-!>yy_%d>
%reduce_arg_rvalue      <yystack_top[%d].yyminor.yy_%d>
%reduce_arg_mvalue      <yystack_top[%d].yymajor>
%reduce_suffix          <}\r\n>
%reduce_init_prefix     <void yy_reduce_init()\r\n{\r\n>
%reduce_init_member     <    reduceprocs[%d] = (reduceProc)yy_reduce_%d;\r\n>
%reduce_init_suffix     <}\r\n>
%destructor_array       <static freeProc freeprocs[%d];\r\n>
%reduce_array           <static reduceProc reduceprocs[%d];\r\n\r\n>
%syntax_error_major     <yymajor>
%syntax_error_minor     <yypminor-!>yy_%d>
%%
/* copy right 2017 by mathew c template example */

%%
/* include header */
#include <assert.h>
#include <memory.h>

void TestFunc()
{
    int a;
    unsigned int c;
    printf("Test !");
}
%%
#ifndef YY_TABLE_H_INCLUDED
#define YY_TABLE_H_INCLUDED
/* header file prefix */

%%

/* header file middle */
%%

/* header file suffix*/
#endif /* YY_TABLE_H_INCLUDED */
%%
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

%%

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

%%
/* syntax error */
static void yy_syntax_error(YY_PARSER *pParser, int yymajor, YY_MINOR_TYPE *yypminor)
{

%%
}

%%
/* parse failed */
static void yy_parse_failed(YY_PARSER *pParser)
{

#ifdef DEBUG
    if(yytraceFile){
        fprintf(yytraceFile,"%sParse Failed!\r\n",yytracePrompt);
    }
#endif

%%
}

%%
/* accept process */
static void yy_accept(YY_PARSER *pParser)
{

#ifdef DEBUG
    if(yytraceFile){
        fprintf(yytraceFile,"%sAccept!\r\n",yytracePrompt);
    }
#endif

    while(pParser->yystack_top >= 0)yy_pop_stack(pParser);
%%
}

%%
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

%%  /* Parse function */
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
%%
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

%%

/* after extra code */

%% /* end */
