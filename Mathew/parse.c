#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>
#include "mathew.h"

/* #define TEST */

#ifdef TEST
#define STATE(a)  psp->state = a; printf("%s:%d:%d    state %d    Token[%s] \n",psp->filename,tok->line,tok->column,a,tokenStrings[tok->type]);
#else
#define STATE(a)  psp->state = a;
#endif

#define INIT_QUEUE(a) InitRingQueue(&psp->rq,sizeof(struct anode),a)
#define RESET_QUEUE() ResetRingQueue(&psp->rq)
#define EN_QUEUE(a) PushRingQueue(&psp->rq,&a)
#define DE_QUEUE(a) PopRingQueue(&psp->rq,a)
#define QUEUE_LENGTH RingQueueLength(&psp->rq)

typedef void (*FailureNote)(int);
static  FailureNote FAILURE_NOTE;

static char* tokenStrings[] =
{
    #define TOKEN(k, s) s,
    #include "token.h"
    #undef  TOKEN
};

typedef void (*Declaration)(struct pstate *psp);
static Declaration    declarations[TK_ID];

PRIVATE void state_waiting_decl_or_rule(struct pstate *psp);
PRIVATE void state_waiting_decl_keyword(struct pstate *psp);
PRIVATE void state_waiting_rule_code(struct pstate *psp);
PRIVATE void state_in_rhs(struct pstate *psp);


/***************************** Read file in buffer *********************/
PRIVATE void read_file(struct pstate *psp, struct input *Input)
{
    FILE *fp;
    char *filebuf;
    int filesize;

    /* Read the input file */
    fp = fopen(psp->filename,"rb");
    if(fp == 0){
        Error_fprintf(CANNOT_READING_FILE,psp->filename);
        psp->err_count++;
        return;
    }
    fseek(fp,0,SEEK_END);
    filesize = ftell(fp);
    fseek(fp,0,SEEK_SET);

    if((Input->bufsize <= 0) || (Input->bufsize >= filesize)){ /* control buffer size range */
        Input->bufsize = filesize;
        Input->proceed = FALSE;
    }else{
        Input->proceed = TRUE;
    }

    filebuf = (char*)Mem_alloc(Input->bufsize + 1); /* Alloc memory store the file content*/
    if(filebuf == 0){
        Error_fprintf(CANNOT_ALLOC_MEM_HOLD_FILE,psp->filename);
        psp->err_count++;
        fclose(fp);
        return;
    }
    if(fread(filebuf,1,Input->bufsize,fp)!=Input->bufsize){
        Error_fprintf(CANNOT_READ_FILE_IN_ALL_BYTES,psp->filename,Input->bufsize);
        Mem_free(filebuf);
        psp->err_count++;
        fclose(fp);
        return;
    }
    if(!Input->proceed){
        fclose(fp);
    }else{
        Input->fp = fp;
    }

    Input->readsize = Input->bufsize;   /* r*/

    filebuf[Input->bufsize] = 0;    /* The end of the character */
    Input->filebuf = Input->cursor = filebuf;

}

/***************************** Resync after error function *********************/
PRIVATE void state_resync_after_error(struct pstate *psp)
{
    struct token *tok_x;

    psp->state = RESYNC_AFTER_ERROR;
    tok_x = GetNextToken();
    while(tok_x->type != TK_END){
        if(tok_x->type == TK_DOT){
            struct token *tok_y;
            tok_y = GetNextToken();
            if(tok_y->type != TK_CODE_BLOCK){ /* If in rules is ".{block}" need jump token "{block}" , if '.' need UnGetNextToken '{block}' */
                UnGetToken(tok_y);
                Error_fprintf(RESYNC_AFTER_ERR,psp->filename,
                              tok_x->line,
                              tok_x->column);
            }else{
                Error_fprintf(RESYNC_AFTER_ERR,psp->filename,
                              tok_y->line,
                              tok_y->column);
            }
            state_waiting_decl_or_rule(psp);
            break;
        }
        if(tok_x->type == TK_DECL){
            Error_fprintf(RESYNC_AFTER_ERR,psp->filename,
                          tok_x->line,
                          tok_x->column);
            state_waiting_decl_keyword(psp);
            break;
        }
        tok_x = GetNextToken();
    }
}

/***************************** Declaration state function *********************/

PRIVATE void state_waiting_type_symbols(struct pstate *psp)
{
    int x;
    struct token *tok;

    tok = GetNextToken();
    STATE(WAITING_TYPE_SYMBOLS);
    x = tok->type;
    if(x == TK_TYPE_BLOCK){
        char *type;
        type = tok->value;
        tok = GetNextToken();
        while (tok->type == TK_ID && islower(tok->value[0])){              /* Nt */
            struct symbol *sp;
            sp = SYMBOL_NEW(tok->value);
            sp->datatype = type;
            tok = GetNextToken();
        }
        if(tok->type != TK_END && tok->type != TK_DOT){
            Error_fprintf(INVALID_TYPE_SYMBOL,psp->filename,
                          tok->line,
                          tok->column,
                          tok->part);
            FAILURE_NOTE(tok->part_column);     /* Print ^-- this */
            psp->err_count++;
            state_resync_after_error(psp);
        }
    }else{
        Error_fprintf(MISSING_TYPE_BLOCK,psp->filename,
                      tok->line,
                      tok->column,
                      psp->declkeyword);
        psp->err_count++;
        state_resync_after_error(psp);
    }
}

PRIVATE void state_waiting_destruct_symbols(struct pstate *psp)
{
    int x;
    int line;
    struct token *tok;

    tok = GetNextToken();
    STATE(WAITING_DESTRUCT_SYMBOLS);
    x = tok->type;
    if(x == TK_CODE_BLOCK){
        char *destruct_code;
        destruct_code = tok->value;
        line = tok->line;
        tok = GetNextToken();
        while (tok->type == TK_ID && islower(tok->value[0])){               /* Nt */
            struct symbol *sp;
            sp = SYMBOL_NEW(tok->value);
            sp->destructor = destruct_code;
            sp->destructor_line = line;
            tok = GetNextToken();
        }
        if(tok->type != TK_END && tok->type != TK_DOT){
            Error_fprintf(INVALID_DESTRUCT_SYMBOL,psp->filename,
                          tok->line,
                          tok->column,
                          tok->part);
            FAILURE_NOTE(tok->part_column);     /* Print ^-- this */
            psp->err_count++;
            state_resync_after_error(psp);
        }
    }else{
        Error_fprintf(MISSING_CODE_BLOCK,psp->filename,
                      tok->line,
                      tok->column,
                      psp->declkeyword);
        psp->err_count++;
        state_resync_after_error(psp);
    }

}

PRIVATE void state_waiting_precedence_symbols(struct pstate *psp)
{
    struct token *tok;

    tok = GetNextToken();
    STATE(WAITING_PRECEDENCE_SYMBOLS);
    while(tok->type == TK_ID && isupper(tok->value[0]))
    {
        struct symbol *sp;
        sp = SYMBOL_NEW(tok->value);
        if(sp->prec>=0){
            Error_fprintf(SYMBOL_ALREADY_HAS_A_PRECEDENCE,
                          psp->filename,
                          tok->line,
                          tok->column,
                          tok->value);
             psp->err_count++;
        }else{
            sp->prec = psp->preccounter;
            sp->assoc = psp->declassoc;
        }
        tok = GetNextToken();
    }
    if(tok->type != TK_END && tok->type != TK_DOT){ /* End of input or '.' finished symbols capture */
        Error_fprintf(CANNOT_ASSIGN_A_PRECEDENCE,psp->filename,
                      tok->line,
                      tok->column,
                      tok->part);
        FAILURE_NOTE(tok->part_column);     /* Print ^-- this */
        psp->err_count++;
        state_resync_after_error(psp);
    }
}

PRIVATE void state_waiting_fallback_symbols(struct pstate *psp)
{
    struct token *tok;
    struct symbol *fallback = 0;

    tok = GetNextToken();
    STATE(WAITING_FALLBACK_SYMBOLS);
    while(tok->type == TK_ID && isupper(tok->value[0]))
    {
        struct symbol *sp;
        sp = SYMBOL_NEW(tok->value);
        if(fallback == 0){
            fallback = sp;
        }else{
            sp->fallback = fallback;
        }
        tok = GetNextToken();
    }
    if(tok->type != TK_END && tok->type != TK_DOT){/* End of input or '.' finished symbols capture */
        Error_fprintf(INVALID_FALLBACK_TOKEN,psp->filename,
                      tok->line,
                      tok->column,
                      tok->part);
        FAILURE_NOTE(tok->part_column);     /* Print ^-- this */
        psp->err_count++;
        state_resync_after_error(psp);
    }
}


PRIVATE void state_waiting_nonterminal(struct pstate *psp)
{
    int x;
    struct token *tok;

    tok = GetNextToken();
    STATE(WAITING_NONTERMINAL);
    x = tok->type;
    if(x == TK_ID && islower(tok->value[0])){
        *(psp->declargslot) = tok->value;
    }else {
        Error_fprintf(NEED_NONTERMINAL_SYMBOL,psp->filename,
                      tok->line,
                      tok->column,
                      tok->part);
        FAILURE_NOTE(tok->part_column);     /* Print ^-- this */
        psp->err_count++;
        state_resync_after_error(psp);
    }
}

PRIVATE void state_waiting_terminal(struct pstate *psp)
{
    int x;
    struct token *tok;

    tok = GetNextToken();
    STATE(WAITING_TERMINAL);
    x = tok->type;
    if(x == TK_ID && isupper(tok->value[0])){
        *(psp->declargslot) = tok->value;
    }else {
        Error_fprintf(NEED_TERMINAL_SYMBOL,psp->filename,
                      tok->line,
                      tok->column,
                      tok->part);
        FAILURE_NOTE(tok->part_column);     /* Print ^-- this */
        psp->err_count++;
        state_resync_after_error(psp);
    }
}

PRIVATE void state_waiting_identifier(struct pstate *psp)
{
    int x; 
    struct token *tok;

    tok = GetNextToken();
    STATE(WAITING_IDENTIFIER);
    x = tok->type;
    if(x == TK_ID){
        *(psp->declargslot) = tok->value;
    }else {
        Error_fprintf(NEED_IDENTIFIER,psp->filename,
                      tok->line,
                      tok->column,
                      tok->part);
        FAILURE_NOTE(tok->part_column);     /* Print ^-- this */
        psp->err_count++;
        state_resync_after_error(psp);
    }
}

PRIVATE void state_waiting_decl_code(struct pstate *psp)
{
    int x;
    struct token *tok;

    tok = GetNextToken();
    STATE(WAITING_DECL_CODE);
    x = tok->type;
    if(x == TK_CODE_BLOCK){
        *(psp->declargslot) = tok->value;
        *(psp->declargslot_line) = tok->line;
    }else{
        Error_fprintf(MISSING_CODE_BLOCK,psp->filename,
                      tok->line,
                      tok->column,
                      psp->declkeyword);
        psp->err_count++;
        state_resync_after_error(psp);
    }
}

PRIVATE void state_waiting_decl_type(struct pstate *psp)
{
    int x;
    struct token *tok;

    tok = GetNextToken();
    STATE(WAITING_DECL_TYPE);
    x = tok->type;
    if(x == TK_TYPE_BLOCK){
        *(psp->declargslot) = tok->value;
    }else{
        Error_fprintf(MISSING_TYPE_BLOCK,psp->filename,
                      tok->line,
                      tok->column,
                      psp->declkeyword);
        psp->err_count++;
        state_resync_after_error(psp);
    }
}
/******************************** Declaration keyword functions ***************************/
PRIVATE void state_decl_accept(struct pstate *psp)
{
    psp->declargslot = &psp->gp->accept;
    psp->declargslot_line = &psp->gp->accept_line;
    state_waiting_decl_code(psp);
}

PRIVATE void state_decl_code(struct pstate *psp)
{
    psp->declargslot = &psp->gp->code;
    psp->declargslot_line = &psp->gp->code_line;
    state_waiting_decl_code(psp);
}

PRIVATE void state_decl_destruct(struct pstate *psp)
{
    state_waiting_destruct_symbols(psp);
}

PRIVATE void state_decl_default_code(struct pstate *psp)
{
    psp->declargslot = &psp->gp->default_code;
    psp->declargslot_line = &psp->gp->default_code_line;
    state_waiting_decl_code(psp);
}

PRIVATE void state_decl_fallback(struct pstate *psp)
{
    state_waiting_fallback_symbols(psp);
}

PRIVATE void state_decl_include(struct pstate *psp)
{
    psp->declargslot = &psp->gp->include;
    psp->declargslot_line = &psp->gp->include_line;
    state_waiting_decl_code(psp);
}

PRIVATE void state_decl_left(struct pstate *psp)
{
    psp->preccounter++;
    psp->declassoc = LEFT;
    state_waiting_precedence_symbols(psp);
}

PRIVATE void state_decl_nonassoc(struct pstate *psp)
{
    psp->preccounter++;
    psp->declassoc = NONASSOC;
    state_waiting_precedence_symbols(psp);
}

PRIVATE void state_decl_nonterminal_type(struct pstate *psp)
{
    psp->declargslot = &psp->gp->nonterminal_type;
    state_waiting_decl_type(psp);
}

PRIVATE void state_decl_nonterminal_destruct(struct pstate *psp)
{
    psp->declargslot = &psp->gp->nonterminal_destruct;
    psp->declargslot_line = &psp->gp->nonterminal_destruct_line;
    state_waiting_decl_code(psp);
}

PRIVATE void state_decl_parse_name(struct pstate *psp)
{
    struct token *tok;
    SetScanState(SPECIAL);    /* Lexer goto special state for special language example @Parse */
    tok = GetNextToken();
    psp->gp->parse_name = tok->value;
    SetScanState(NORMAL);     /* Turn back to normal state */
}

PRIVATE void state_decl_parse_failed(struct pstate *psp)
{
    psp->declargslot = &psp->gp->failed;
    psp->declargslot_line = &psp->gp->failed_line;
    state_waiting_decl_code(psp);
}

PRIVATE void state_decl_right(struct pstate *psp)
{
    psp->preccounter++;
    psp->declassoc = RIGHT;
    state_waiting_precedence_symbols(psp);
}

PRIVATE void state_decl_start_symbol(struct pstate *psp)
{
    psp->declargslot = &psp->gp->start;
    state_waiting_nonterminal(psp);
}

PRIVATE void state_decl_syntax_error(struct pstate *psp)
{
    psp->declargslot = &psp->gp->error;
    psp->declargslot_line = &psp->gp->error_line;
    state_waiting_decl_code(psp);
}

PRIVATE void state_decl_type(struct pstate *psp)
{
    state_waiting_type_symbols(psp);
}

PRIVATE void state_decl_token_type(struct pstate *psp)
{
    psp->declargslot = &psp->gp->token_type;
    state_waiting_decl_type(psp);
}

PRIVATE void state_decl_token_destruct(struct pstate *psp)
{
    psp->declargslot = &psp->gp->token_destruct;
    psp->declargslot_line = &psp->gp->token_destruct_line;
    state_waiting_decl_code(psp);
}

PRIVATE void state_decl_token_prefix(struct pstate *psp)
{
    struct token *tok;
    SetScanState(SPECIAL);    /* Lexer goto special state for special language example @TOK_ */
    tok = GetNextToken();
    psp->gp->token_prefix = tok->value;
    SetScanState(NORMAL);     /* Turn back to normal state */
}

/***************************** Declare keword switch function *********************/
PRIVATE void state_waiting_decl_keyword(struct pstate *psp)
{
    int x;
    struct token *tok;

    tok = GetNextToken();
    STATE(WAITING_DECL_KEYWORD);
    x = tok->type;
    psp->declkeyword = tokenStrings[x];

    if(x < TK_ID && declarations[x] != 0){
        (*declarations[x])(psp);
    }else{
        Error_fprintf(INVALID_DECL_KEYWORD,psp->filename,
                      tok->line,
                      tok->column,
                      tok->value); /* Token location position to space string */
        psp->err_count++;
        state_resync_after_error(psp);
    }

    x = GetToken()->type;
    if(x != TK_END)
        state_waiting_decl_or_rule(psp);              /* Need goto first state and avoid multiple end of input*/
}

/******************************** Rule function *********************************/
PRIVATE void state_rhs_alias_2(struct pstate *psp)
{
    int x;
    struct token *tok;

    tok = GetNextToken();

    STATE(RHS_ALIAS_2);
    x = tok->type;
    if(x == TK_RPAREN){
        state_in_rhs(psp);
    }else{
        Error_fprintf(LOST_RPAREN,psp->filename,
                      tok->line,
                      tok->column,
                      tok->part);
        FAILURE_NOTE(tok->part_column);     /* Print ^-- this */
        psp->err_count++;
        state_resync_after_error(psp);
    }
}

PRIVATE void state_rhs_alias_1(struct pstate *psp)
{
    int x;
    struct token *tok;

    tok = GetNextToken();
    STATE(RHS_ALIAS_1);
    x = tok->type;
    if(x == TK_ALIAS){
        int i;
        BOOL not_conflict = TRUE;
        /* alias conflict */
        if(psp->lhsalias && strcmp(tok->value,psp->lhsalias) == 0){
            Error_fprintf(ALIAS_CONFLICT,psp->filename,
                          tok->line,
                          tok->column,
                          tok->part);
            FAILURE_NOTE(tok->part_column);     /* Print ^-- this */
            not_conflict = FALSE;
        }
        for(i = 0; i < psp->nrhs - 1; i++){
            if(psp->rhsalias[i] && strcmp(tok->value,psp->rhsalias[i]) == 0){

                Error_fprintf(ALIAS_CONFLICT,psp->filename,
                              tok->line,
                              tok->column,
                              tok->part);
                FAILURE_NOTE(tok->part_column);     /* Print ^-- this */
                not_conflict = FALSE;
            }  
        }
        if(not_conflict)psp->rhsalias[psp->nrhs-1] = tok->value; /* TOKEN($ALIAS1)($ALIAS2) will used $ALIAS2*/
        state_rhs_alias_2(psp);
    }else{
        Error_fprintf(INVALID_ALIAS,psp->filename,
                      tok->line,
                      tok->column,
                      tok->part);
        FAILURE_NOTE(tok->part_column);     /* Print ^-- this */
        psp->err_count++;
        state_resync_after_error(psp);
    }
}

PRIVATE void state_precedence_mark_2(struct pstate *psp)
{
    int x;
    struct token *tok;

    tok = GetNextToken();
    STATE(PRECEDENCE_MARK_2);
    x = tok->type;
    if(x == TK_RBRACKET){
        state_waiting_rule_code(psp);
    }else{
        Error_fprintf(LOST_RBRACKET,psp->filename,
                      tok->line,
                      tok->column,
                      tok->part);
        FAILURE_NOTE(tok->part_column);     /* Print ^-- this */
        psp->err_count++;
        state_resync_after_error(psp);
    }
}

PRIVATE void state_precedence_mark_1(struct pstate *psp)
{
    int x;
    struct token *tok;

    tok = GetNextToken();
    STATE(PRECEDENCE_MARK_1);
    x = tok->type;

    if(x == TK_ID && isupper(tok->value[0]))
    {
        if(psp->prevrule == 0){
           Error_fprintf(MISSING_PREVIOUS_RULE,psp->filename,
                         tok->line,
                         tok->column,
                         tok->part);
           FAILURE_NOTE(tok->part_column);     /* Print ^-- this */
           psp->err_count++;
           state_resync_after_error(psp);
        } else if(psp->prevrule->precsymbol!=0){
             Error_fprintf(ALREADY_HAS_A_PRECEDENCE_SYMBOL,psp->filename,
                           tok->line,
                           tok->column,
                           tok->part);
             FAILURE_NOTE(tok->part_column);     /* Print ^-- this */
             psp->err_count++;
             state_resync_after_error(psp);
        }else{
            psp->prevrule->precsymbol = SYMBOL_NEW(tok->value);
            state_precedence_mark_2(psp);
        }
    }else{
        Error_fprintf(NEED_TERMINAL_SYMBOL,psp->filename,
                      tok->line,
                      tok->column,
                      tok->part);
        FAILURE_NOTE(tok->part_column);     /* Print ^-- this */
        psp->err_count++;
        state_resync_after_error(psp);
    }

}

PRIVATE void state_waiting_rule_code(struct pstate *psp)
{
    int x;
    struct token *tok;

    tok = GetNextToken();
    STATE(WAITING_RULE_CODE);
    x = tok->type;
    if(x == TK_CODE_BLOCK){
        if(psp->prevrule == 0){
           Error_fprintf(MISSING_PREVIOUS_RULE,psp->filename,
                         tok->line,
                         tok->column,
                         tok->part);
           FAILURE_NOTE(tok->part_column);     /* Print ^-- this */
           psp->err_count++;
           state_resync_after_error(psp);
        }else if(psp->prevrule->code!=0){
           Error_fprintf(RULE_CODE_ALREADY_EXISTED,psp->filename,
                         tok->line,
                         tok->column,
                         tok->part);
           FAILURE_NOTE(tok->part_column);     /* Print ^-- this */
           psp->err_count++;
           state_resync_after_error(psp);
        }else{
           psp->prevrule->code = tok->value;
           psp->prevrule->code_line = tok->line;
           /* psp->prevrule = 0; */
        }
        state_waiting_decl_or_rule(psp);    /* Need goto first state */
    }else if(x == TK_LBRACKET){
        state_precedence_mark_1(psp);
    }else{
        UnGetToken(tok);
        state_waiting_decl_or_rule(psp);
    }
}
PRIVATE void addInsertAction(struct pstate *psp, int n, char *code, int code_line)
{
    /* Insert action */
    char s[10];
    struct symbol *sp;
    struct rule *rp;
    do{
       sprintf(s,"nt_%06d",psp->action_index++);
       sp = Symbol_find(s);
    }while(sp!=0);
    sp = SYMBOL_NEW(s);
    sp->epsilon = TRUE;               /* Insert action is a epsilon rule symbol */
    MEM_CHECK(sp);
    /* Malloc memory */
    rp = (struct rule *)Mem_alloc(sizeof(struct rule));
    MEM_CHECK(rp);
    /* Initialize rule */
    rp->lhs = sp;
    rp->lhsalias = 0;
    rp->nrhs = 0;
    rp->code  = code;
    rp->code_line = code_line;
    rp->reduce_proc_num = -1;
    rp->precsymbol = 0;
    rp->nextlhs = rp->lhs->rule;     /* New rule The same left hand symbol */
    rp->lhs->rule = rp;              /* Symbol head rule is the best new rule*/
    rp->next = 0;
    rp->used = FALSE;
    rp->index = psp->gp->rule_num++; /* In global rule link in the tail */
    /* Link global rule list */
    if(psp->firstrule != 0){
        psp->lastrule->next = rp;
        psp->lastrule = rp;
    }else{
        psp->firstrule = psp->lastrule = rp;
    }

    psp->rhs[n] = sp;
}

PRIVATE void state_in_rhs(struct pstate *psp)
{
    int x;
    struct token *tok;

    tok = GetNextToken();
    STATE(IN_RHS);
    x = tok->type;
    if(x == TK_DOT){
        struct rule *rp;
        int i;
        /* Malloc memory */
        rp = (struct rule *)Mem_alloc(sizeof(struct rule) +
                                      sizeof(struct symbol*)*psp->nrhs +
                                      sizeof(char *)*psp->nrhs);
        MEM_CHECK(rp);
        rp->rhs = (struct symbol **)&rp[1];
        rp->rhsalias = (char **)&(rp->rhs[psp->nrhs]);
        /* Initialize rule */
        rp->lhs = psp->lhs;
        rp->lhsalias = psp->lhsalias;
        rp->nrhs = psp->nrhs;
        rp->code  = 0;
        rp->reduce_proc_num = -1;
        rp->precsymbol = 0;
        rp->nextlhs = rp->lhs->rule;     /* New rule The same left hand symbol */
        rp->lhs->rule = rp;              /* Symbol head rule is the best new rule*/
        rp->next = 0;
        rp->used = FALSE;
        rp->index = psp->gp->rule_num++; /* In global rule link in the tail */
        /* Link global rule list */
        if(psp->firstrule != 0){
            psp->lastrule->next = rp;
            psp->lastrule = rp;
        }else{
            psp->firstrule = psp->lastrule = rp;
        }

        /* Back fill insert actions */
        if(QUEUE_LENGTH){
            struct anode insert_action;
            while(DE_QUEUE(&insert_action)){
                addInsertAction(psp,insert_action.n,insert_action.code,insert_action.code_line);
            }
        }
        /* Need after back fill insert actions */
        for(i = 0; i<psp->nrhs; i++){
            rp->rhs[i] = psp->rhs[i];
            rp->rhsalias[i] = psp->rhsalias[i];
        }

        psp->prevrule = rp;              /* For next state see '{' add code */
        if(rp->nrhs == 0)rp->lhs->epsilon = TRUE;

        state_waiting_rule_code(psp);
    }else if(x == TK_ID && isalpha(tok->value[0])){
        /* Automatic growth */
        if(psp->nrhs >= psp->maxrhs){
            psp->maxrhs = psp->maxrhs*2;
            psp->rhs = (struct symbol **)Mem_alloc(sizeof(struct symbol *)*psp->maxrhs);
            psp->rhsalias = (char **)Mem_alloc(sizeof(char *)*psp->maxrhs);
            MEM_CHECK(psp->rhs);
            MEM_CHECK(psp->rhsalias);
        }
        psp->rhs[psp->nrhs] = SYMBOL_NEW(tok->value);
        psp->rhsalias[psp->nrhs] = 0;
        psp->nrhs++;
        state_in_rhs(psp);
    }else if(x == TK_LPAREN && psp->nrhs > 0){
        state_rhs_alias_1(psp);
    }else if(x == TK_CODE_BLOCK){
        /* Record how many insert action */
        struct anode insert_action;
        insert_action.n = psp->nrhs;
        insert_action.code = tok->value;
        insert_action.code_line = tok->line;
        EN_QUEUE(insert_action);

        /* To make empty space */
        if(psp->nrhs >= psp->maxrhs){
            psp->maxrhs = psp->maxrhs*2;
            psp->rhs = (struct symbol **)Mem_alloc(sizeof(struct symbol *)*psp->maxrhs);
            psp->rhsalias = (char **)Mem_alloc(sizeof(char *)*psp->maxrhs);
            MEM_CHECK(psp->rhs);
            MEM_CHECK(psp->rhsalias);
        }

        psp->rhs[psp->nrhs] = 0;
        psp->rhsalias[psp->nrhs] = 0;
        psp->nrhs++;

        state_in_rhs(psp);
    }else{
        Error_fprintf(UNRECOGNIZED_INPUT,psp->filename,
                      tok->line,
                      tok->column,
                      tok->part);
        FAILURE_NOTE(tok->part_column);     /* Print ^-- this */
        psp->err_count++;
        state_resync_after_error(psp);
    }
}

PRIVATE void state_lhs_alias_3(struct pstate *psp)
{
    int x;
    struct token *tok;

    tok = GetNextToken();
    STATE(LHS_ALIAS_3);
    x = tok->type;
    if(x == TK_ARROW){
        state_in_rhs(psp);
    }else{
        Error_fprintf(LOST_ARROW,psp->filename,
                      tok->line,
                      tok->column,
                      tok->part);
        FAILURE_NOTE(tok->part_column);     /* Print ^-- this */
        psp->err_count++;
        state_resync_after_error(psp);
    }
}

PRIVATE void state_lhs_alias_2(struct pstate *psp)
{
    int x;
    struct token *tok;

    tok = GetNextToken();
    STATE(LHS_ALIAS_2);
    x = tok->type;
    if(x == TK_RPAREN){
        state_lhs_alias_3(psp);
    }else{
        Error_fprintf(LOST_RPAREN,psp->filename,
                      tok->line,
                      tok->column,
                      tok->part);
        FAILURE_NOTE(tok->part_column);     /* Print ^-- this */
        psp->err_count++;
        state_resync_after_error(psp);
    }
}

PRIVATE void state_lhs_alias_1(struct pstate *psp)
{
    int x;
    struct token *tok;

    tok = GetNextToken();
    STATE(LHS_ALIAS_1);
    x = tok->type;
    if(x == TK_ALIAS){
        psp->lhsalias = tok->value;
        state_lhs_alias_2(psp);
    }else{
        Error_fprintf(INVALID_ALIAS,psp->filename,
                      tok->line,
                      tok->column,
                      tok->part);
        FAILURE_NOTE(tok->part_column);     /* Print ^-- this */
        psp->err_count++;
        state_resync_after_error(psp);
    }
}

PRIVATE void state_waiting_arrow(struct pstate *psp)
{
    int x;
    struct token *tok;

    tok = GetNextToken();
    STATE(WAITING_ARROW);
    x = tok->type;
    if(x == TK_ARROW){
        state_in_rhs(psp);
    }else if(x == TK_LPAREN){
        state_lhs_alias_1(psp);
    }else{
        Error_fprintf(EXPECTED_ARROW_OR_LPAREN,psp->filename,
                      tok->line,
                      tok->column,
                      tok->part);
        FAILURE_NOTE(tok->part_column);     /* Print ^-- this */
        psp->err_count++;
        state_resync_after_error(psp);
    }
}

/***************************** Declare or rule switch function *******************/
PRIVATE void state_waiting_decl_or_rule(struct pstate *psp)
{
    int x;
    struct token *tok;

    tok = GetNextToken();

    STATE(WAITING_DECL_OR_RULE);
    x = tok->type;
    if(x == TK_END){
        return;
    }else if(x == TK_DECL){
        state_waiting_decl_keyword(psp);
    }else if(x <= TK_ID && islower(tok->value[0])){
        psp->lhs = SYMBOL_NEW(tok->value);
        psp->nrhs = 0;
        psp->lhsalias = 0;
        psp->prevrule = 0;
        RESET_QUEUE();
        state_waiting_arrow(psp);
    }else{
        Error_fprintf(NOT_DECL_OR_RULE,psp->filename,
                      tok->line,
                      tok->column,
                      tok->part);
        FAILURE_NOTE(tok->part_column);     /* Print ^-- this */
        psp->err_count++;
        state_resync_after_error(psp);
        /* state_waiting_decl_or_rule(psp); */ /* Eate token before it can be parse */
    }
}


PRIVATE void state_initialize(struct pstate *psp)
{
     int i;
     psp->state = INITIALIZE;
     psp->action_index = 1;
     psp->preccounter = 0;
     psp->prevrule = 0;
     psp->firstrule = psp->lastrule = 0;
     psp->gp->rule_num = 0;
     psp->maxrhs = MAXRHS;
     psp->rhs = (struct symbol **)Mem_alloc(sizeof(struct symbol *)*psp->maxrhs);
     psp->rhsalias = (char **)Mem_alloc(sizeof(char *)*psp->maxrhs);
     MEM_CHECK(psp->rhs);
     MEM_CHECK(psp->rhsalias);
     INIT_QUEUE(64);                         /* Initialize Insert Action queue */
     /* Initialize declaration keyword functions */
     for (i = 0; i < TK_ID; i++){
         declarations[i] = 0;
     }

     declarations[TK_ACCEPT]               = state_decl_accept;
     declarations[TK_CODE]                 = state_decl_code;
     declarations[TK_DESTRUCT]             = state_decl_destruct;
     declarations[TK_DFAULT_CODE]          = state_decl_default_code;
     declarations[TK_FALLBACK]             = state_decl_fallback;
     declarations[TK_INCLUDE]              = state_decl_include;
     declarations[TK_LEFT]                 = state_decl_left;
     declarations[TK_NONASSOC]             = state_decl_nonassoc;
     declarations[TK_NONTERMINAL_TYPE]     = state_decl_nonterminal_type;
     declarations[TK_NONTERMINAL_DESTRUCT] = state_decl_nonterminal_destruct;
     declarations[TK_PARSE_NAME]           = state_decl_parse_name;
     declarations[TK_PARSE_FAILED]         = state_decl_parse_failed;
     declarations[TK_RIGHT]                = state_decl_right;
     declarations[TK_START_SYMBOL]         = state_decl_start_symbol;
     declarations[TK_SYNTAX_ERROR]         = state_decl_syntax_error;
     declarations[TK_TYPE]                 = state_decl_type;
     declarations[TK_TOKEN_TYPE]           = state_decl_token_type;
     declarations[TK_TOKEN_DESTRUCT]       = state_decl_token_destruct;
     declarations[TK_TOKEN_PREFIX]         = state_decl_token_prefix;


     state_waiting_decl_or_rule(psp);
}


void Parse(struct global *gp)
{
     struct pstate ps;
     struct input Input;

     ps.gp           = gp;
     ps.filename     = gp->filename;
     ps.err_count    = 0;

     Input.fp        = 0;
     Input.filename  = ps.filename;
     Input.exclude   = 0;
     Input.readsize  = 0;
     Input.column    = 1;
     Input.line      = 1;
     Input.mastartln = 1;
     Input.macolumn  = 1;
     Input.madefine  = gp->madefine;
     Input.bufsize   = gp->opt->bufsize;

     if(Input.bufsize > 0 && Input.bufsize < 32)
         Input.bufsize = 32;                  /* minimum buffer size */

     read_file(&ps,&Input);                   /* Initialize file buffer */

     Tplstring_init(&Input.tplstr,256);

     InitLexer(&Input);                       /* Initialize lexer */

     FAILURE_NOTE = PrintFailureNote;

     if(gp->opt->hiddenflag){
         FAILURE_NOTE = EmptyFailureNote;
         SetPartState(HIDDEN);
     }

     state_initialize(&ps);

     Tplstring_free(&Input.tplstr);
     free(Input.filebuf);                     /* Release the buffer after parsing */

     gp->rule = ps.firstrule;
     gp->lastrule = ps.lastrule;
     gp->err_count = ps.err_count;

     if(Input.fp)fclose(Input.fp);
     if(Input.exclude > 0){
         Error_fprintf(MISSING_DEF_END,Input.mastartln,Input.macolumn);
         exit(1);
     }

}

