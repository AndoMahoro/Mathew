#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>
#include "mathew.h"

#define NEXT(a) (*(char**)(((unsigned long)a)+offset))
#define CHECK_ACTION(a) assert(a->type == REDUCE || a->type == RD_RESOLVED || a->type == CONFLICT)

static struct symbol *symbol_freelist = 0;
static struct symbol *symbol_list = 0;
static int symbol_index = 0;

/* Return a pointer to a new symbol */
PRIVATE struct symbol *newsymbol()
{
   struct symbol *newsp;

   if(symbol_freelist == 0){
        int i;
        int amt = 4;
        symbol_freelist = (struct symbol *)Mem_alloc( sizeof(struct symbol) * amt);
        MEM_CHECK(symbol_freelist);
        for(i = 0; i < amt-1; i++){
            symbol_freelist[i].next = &symbol_freelist[i+1];
        }
        symbol_freelist[amt-1].next = 0;
    }   
    newsp = symbol_freelist;
    symbol_freelist = symbol_freelist->next;

    return newsp;
}

char *Make_filename(char *name, char *suffix)
{
    char *filename;
    char *cp;
    unsigned long len;
   
    cp = strrchr(name,'.');
    if(cp == 0){
        cp = name + strlen(name); /* No suffix file */
    }
    len = (unsigned long)(cp - name); 
    filename = (char *)Mem_alloc(len + strlen(suffix) + 1);
    MEM_CHECK(filename);
    strncpy(filename,name,len);
    filename[len] = 0 ;
    strcat(filename,suffix);
    return filename;
}

/* Symbol compare function 1 */
int Symbolcmp_1(struct symbol *s1,char *s2)
{
    return strcmp(s1->name,s2);
}

/* Symbol compare function 2 */
int Symbolcmp_2(struct symbol *s1,struct symbol *s2)
{
    return strcmp(s1->name,s2->name);
}

/* Symbol compare function 3 for symbol array sort */
int Symbolcmp_3(struct symbol *s1,struct symbol *s2)
{
    int i1 = (*s1).index + 100000000*((int)(*s1).type); /* Magnified 10000000 times */
    int i2 = (*s2).index + 100000000*((int)(*s2).type);
    return i1 - i2;
}

/* State compare function */
int Statecmp(struct state *s1, struct state *s2)
{
  int rc;
  struct config *a,*b;

  a = s1->bp;
  b = s2->bp;

  for(rc=0; rc==0 && a && b;  a=a->bp, b=b->bp){
    rc = a->rp->index - b->rp->index;
    if( rc==0 ) rc = a->dot - b->dot;
  }
  if(rc == 0){
    if(a) rc = 1;
    if(b) rc = -1;
  }
  return rc;
}

/* Configuration compare function */
int Configcmp(struct config *c1, struct config *c2)
{
    int x;
    x = c1->rp->index - c2->rp->index;
    if(x == 0) x = c1->dot - c2->dot;
    return x;
}

/* Action compare function */
int Actioncmp(struct action *ap1, struct action *ap2)
{
    int ac;
    ac = ap1->forward->index - ap2->forward ->index;
    if(ac == 0)ac = (int)ap1->type - (int)ap2->type;
    if(ac == 0){
        CHECK_ACTION(ap1);
        CHECK_ACTION(ap2);
        ac = ap1->x.rp->index - ap2->x.rp->index;
    }
    return ac;
}

/* Return a pointer to the string "str".
** Create a new memory to store the string
** if this is the first time "str" has been seen.
*/
char *StrSafe(char *str)
{
    char *z;
    z = Str_find(str);
    if(z == 0 &&(z=Mem_alloc(strlen(str)+1))!=0){
        strcpy(z,str);
        Str_insert(z);
    }
    MEM_CHECK(z);
    return z;
}


/* Return a pointer to the (terminal or nonterminal) symbol "x".
** Create a new symbol if this is the first time "x" has been seen.
*/
struct symbol *Symbol_new(char *x, int type)
{
  struct symbol *sp;

  sp = Symbol_find(x);
  if(sp == 0){
    sp = newsymbol();
    MEM_CHECK(sp);
    sp->index = symbol_index++;
    sp->name = StrSafe(x);
    sp->type = type;
    sp->rule = 0;
    sp->fallback = 0;
    sp->prec = -1;
    sp->firstset.size  = 0;
    sp->firstset.value = 0;
    sp->assoc = UNKNOW;
    sp->epsilon = FALSE;
    sp->destructor = 0;
    sp->destructor_line = -1;
    sp->destructor_proc_num = -1;
    sp->datatype = 0;
    sp->next = 0;          /* Equal newsymbol() newsp->next = 0; */
    Symbol_insert(sp);
    Insertitem((char**)&symbol_list,(char**)&symbol_list->next,(char*)sp,Symbolcmp_3);
  }
  return sp;
}

struct symbol *GetOrderlySymbols()
{
    return symbol_list;
}

void ResetSymbolsIndex(struct symbol *symbols)
{
    int i = 0;
    struct symbol *sp = symbols;
    while(sp){
        sp->index = i++;
        sp = sp->next;
    }
}

void Symbol_arrayof(struct symbol ***sp_array,struct symbol *symbols)
{
    int i = 0;
    struct symbol *sp = symbols;
    while(sp){
        (*sp_array)[i++] = sp;
        sp = sp->next;
    }
}

void FindStartSymbol(struct global *gp)
{
    int i, create_newstart = 0;
    struct rule *rp;
    /* Set start symbol */
    if(gp->start){
       gp->start_symbol = Symbol_find(gp->start);
       if(gp->start_symbol == 0){
           gp->err_count++;
           gp->start_symbol = gp->rule->lhs;
           Error_fprintf(NOT_FIND_SPECIFIY_START_SYMBOL,gp->start,gp->start_symbol->name);
       }
    }else{
       gp->start_symbol = gp->rule->lhs;
    }

    /*Make sure the start symbol doesn't occur on the right hand if occur create a new start symbol */
    for(rp = gp->rule; rp; rp = rp->next){
        for(i = 0; i < rp->nrhs; i++){
            if(rp->rhs[i] == gp->start_symbol){
                create_newstart = 1;
                break;
            }
        }
        if(create_newstart) break;
    }

    /* Create a new start symbol */
    if(create_newstart){
        char s[10];
        struct symbol *sp;
        struct rule *rp, *nrp;
        i = 1;
        do{
            sprintf(s,"start_%03d",i++);
            sp = Symbol_find(s);
        }while(sp!=0);
        Error_fprintf(START_SYMBOL_APPEAR_RULE_RIGHT,gp->start_symbol->name,s);
        sp = SYMBOL_NEW(s);
        MEM_CHECK(sp);
        /* Malloc memory */
        rp = (struct rule *)Mem_alloc(sizeof(struct rule) +
                                              sizeof(struct symbol*)*1 +
                                              sizeof(char *)*1);
        MEM_CHECK(rp);
        rp->rhs = (struct symbol **)&rp[1];
        rp->rhsalias = (char **)&(rp->rhs[1]);
        /* Initialize rule */
        rp->lhs = sp;
        /* sp->rule = rp;  symbol link rule in rp->lhs->rule = rp; */
        rp->lhsalias = 0;
        rp->nrhs = 1;
        rp->code = 0;
        rp->reduce_proc_num = -1;
        rp->precsymbol = 0;
        rp->nextlhs = rp->lhs->rule;     /* New rule The same left hand symbol */
        rp->lhs->rule = rp;              /* Symbol head rule is the best new rule*/
        rp->used = FALSE;
        rp->index = 0;                   /* In global rule link in the head index in the tail */
        rp->rhs[0] = gp->start_symbol;
        rp->rhsalias[0] = 0;             /* Parse.c will obligate one sit */
        /* Link global rule list and redirect rule index */
        for(nrp = gp->rule; nrp; nrp = nrp->next){
            nrp->index++;
        }
        rp->next = gp->rule;
        gp->rule = rp;
        gp->start_symbol = sp;
        gp->rule_num++;
    }
}

int Terminal_count(struct global *gp)
{ 
    int i = 0;
    struct symbol *sp = gp->symbols;
    while(sp && sp->type  == TERMINAL){
        i++;
        sp = sp->next;
    }
    return i;
}
/* If symbol no have rule that will add default rule (symbol ->. ) */
struct rule *ProcessNoRuleSymbol(struct global *gp, struct symbol *sp)
{
    if(sp->rule == 0){
        struct rule *rp;
        rp = (struct rule *)Mem_alloc(sizeof(struct rule));
        MEM_CHECK(rp);
        /* Initialize rule */
        rp->lhs = sp;
        rp->lhsalias = 0;
        rp->nrhs = 0;
        rp->code = 0;
        rp->reduce_proc_num = -1;
        rp->precsymbol = 0;
        rp->nextlhs = rp->lhs->rule;     /* New rule The same left hand symbol */
        rp->lhs->rule = rp;              /* Symbol head rule is the best new rule*/
        rp->next = 0;
        rp->used = FALSE;
        rp->index = gp->rule_num++; /* In global rule link in the tail */
        /* Link global rule list */
        gp->lastrule->next = rp;
        gp->lastrule = rp;
    }
    return sp->rule;
}

void Reprint(struct global *gp)
{
    struct rule   *rp;
    struct symbol *sp;
    int i,j,maxlen,len,ncolumns,skip;

    FILE *out = GetOutputStream();
    Output_fprintf(REPRINT_INPUT_FILE,gp->filename);

    maxlen = 10;
    sp = gp->symbols;
    while(sp){
        len = strlen(sp->name);
        if(len > maxlen)maxlen = len;
        sp = sp->next;
    }

    ncolumns = 76 /( maxlen + 5);
    if( ncolumns < 1 ) ncolumns = 1;
    skip = (gp->symbol_num + ncolumns - 1) / ncolumns;
    for(i = 0; i < skip; i++){
        fprintf(out,"*");
        for(j = i; j < gp->symbol_num ; j += skip){
            sp = gp->symbol_array[j];
            assert( sp->index == j );
            fprintf(out," %3d %-*.*s", j, maxlen, maxlen, sp->name);
        }
        fprintf(out,"\n");
    }
    fprintf(out,"*/\n");

    for(rp = gp->rule; rp; rp = rp->next){
        fprintf(out,"%s", rp->lhs->name);
        /* if( rp->lhsalias )fprintf(out,"(%s)", rp->lhsalias); */
        fprintf(out," ->");
        for(i = 0; i< rp->nrhs; i++){
            fprintf(out," %s", rp->rhs[i]->name);
            /* if( rp->rhsalias[i] )fprintf(out,"(%s)", rp->rhsalias[i]); */
        }
        fprintf(out,".");
        /* if( rp->precsymbol )fprintf(out," [%s]", rp->precsymbol->name); */
        /* if( rp->code )fprintf(out,"\n    %s", rp->code); */
        fprintf(out,"\n");
    }
    fprintf(out,"\n");

}

void SetRluesPrecedence(struct global *gp)
{
    struct rule *rp;
    for(rp = gp->rule; rp; rp = rp->next){
        if(rp->precsymbol == 0){
            int i;
            for(i = rp->nrhs - 1; i >= 0 ; i--){
                if(rp->rhs[i]->prec >= 0){
                    rp->precsymbol = rp->rhs[i];
                    break;
                }
            }
        }
    }

}

void ResetRulesUsed(struct global *gp)
{
    struct rule *rp = gp->rule;
    while(rp){
        rp->used = FALSE;
        rp = rp->next;
    }
}

/* Print used rule */
void PrintUsedRules(struct global *gp)
{
    int i;
    struct rule *rp = gp->rule;

    FILE *out = GetOutputStream();
    Output_fprintf(USED_RULE);

    while(rp){
        if(rp->used == TRUE){
            fprintf(out,"%s", rp->lhs->name);
            fprintf(out," ->");
            for(i = 0; i< rp->nrhs; i++){
                fprintf(out," %s", rp->rhs[i]->name);
            }
            fprintf(out,".");
            fprintf(out,"\n");
        }
        rp = rp->next;
    }

    fprintf(out,"\n");
}

/* Print no used rule */
void PrintNoUsedRules(struct global *gp)
{
    int i;
    struct rule *rp = gp->rule;
    FILE *out = GetOutputStream();
    Output_fprintf(NO_USED_RULE);
    while(rp){
        if(rp->used == FALSE){
            fprintf(out,"%s", rp->lhs->name);
            fprintf(out," ->");
            for(i = 0; i< rp->nrhs; i++){
                fprintf(out," %s", rp->rhs[i]->name);
            }
            fprintf(out,".");
            fprintf(out,"\n");
        }
        rp = rp->next;
    }
    fprintf(out,"\n");
}

void PrintUsedRuleEpsilonSymbols(struct global *gp)
{
    struct rule *rp =gp->rule; 
    FILE *out = GetOutputStream();
    Output_fprintf(USED_RULE_EPSILON_SYMBOLS);
    while(rp)
    {
        if(rp->used == TRUE && rp->lhs->epsilon == TRUE){
            Output_fprintf(SYMBOL_AT_RULE,rp->index, rp->lhs->name);
        }
        rp = rp->next;
    }
    fprintf(out,"\n");
}

void PrintNonterminalsFirstSet(struct global *gp)
{
    int i,j;
    SEG *firstset;
    struct symbol *sp;

    FILE *out = GetOutputStream();
    Output_fprintf(NT_SYMBOLS_FIRST_SET);

    for(i = gp->terminal_num; i < gp->symbol_num; i++){
        sp = gp->symbol_array[i];
        firstset = sp->firstset.value;
        if(firstset){
            fprintf(out,"%-10s::= ",sp->name);
            for(j = 0; j < gp->terminal_num; j++){
                if(BITISSET(firstset,j)){
                    fprintf(out,"%s ",gp->symbol_array[j]->name);
                }
            }
            if(sp->epsilon){
                fprintf(out,"(epsilon) ");
            }
            fprintf(out,"\n");
        }
    }
    fprintf(out,"\n");
}

void ConfigPrint(FILE *fp, struct config *cfp)
{
    struct rule *rp;
    int i;
    rp = cfp->rp;
    fprintf(fp,"%s -> ",rp->lhs->name);
    for(i = 0; i < rp->nrhs; i++){
        if(i == cfp->dot)fprintf(fp,"* ");
        fprintf(fp,"%s ",rp->rhs[i]->name);
    }
    fprintf(fp,".");
}

void SetPrint(FILE *fp, struct set s, struct global *gp)
{
    int i;
    char *specer = "";
    fprintf(fp,"{");
    for(i = 0; i < gp->terminal_num; i++){
        if(BITISSET(s.value,i)){
            fprintf(fp,"%s%s",specer,gp->symbol_array[i]->name);
            specer = ",";
        }
    }
    fprintf(fp,"}");
}

void PlinkPrint(FILE *fp, struct plink *plp, char *tag)
{
    while(plp){
        fprintf(fp,"%20s%s(state %03d) ","",tag,plp->cfp->stp->index);
        ConfigPrint(fp,plp->cfp);
        fprintf(fp,"\r\n");
        plp = plp->next;
    }
}

int ActionPrint(FILE *fp, struct action *ap,int align)
{
    int result = 1;
    if(ap->type == SHIFT){
        fprintf(fp,"%*s shift %d",align,ap->forward->name,ap->x.stp->index);
    }else if(ap->type == REDUCE){
        fprintf(fp,"%*s reduce %d",align,ap->forward->name,ap->x.rp->index);
    }else if(ap->type == ACCEPT){
        fprintf(fp,"%*s accept",align,ap->forward->name);
    }else if(ap->type == CONFLICT){
        fprintf(fp,"%*s reduce %-03d Parsing conflict.",align,ap->forward->name,ap->x.rp->index);
    }else{
        result = 0;
    }
    return result;
}

/* Orderly list insert item */
void Insertitem(char **list, char **next, char *item, int(*comparefunc)())
{
    unsigned long offset;
    char *cp,*pre;

    /* Insert item into the empty list */
    if(*list == 0){
        *list = item;
        return;
    }

    offset = (unsigned long)next - (unsigned long)(*list); /* Struct head to next point offset */

    if((*comparefunc)(item,*list) < 0)
    {
        NEXT(item) = *list;
        *list = item;
        return;
    }

    pre = *list;
    cp = NEXT(*list);
    while(cp){
        if((*comparefunc)(item,cp) <= 0){
            NEXT(pre) = item;
            NEXT(item) = cp;
            break;
        }
        pre = cp;
        cp = NEXT(cp);
    }

    if(cp == 0){
        NEXT(pre) = item;
    }

}

/* List insert sort */
char *Insertsort(char *list, char **next, int(*comparefunc)())
{
    unsigned long offset;
    char *new_list;
    char *ep,*cp,*pcp;


    if(list == 0)return 0;
    offset = (unsigned long)next - (unsigned long)list; /* Struct head to next point offset */

    new_list = list;
    list = NEXT(list);
    NEXT(new_list) = 0;
    while(list){
        /* Take list node */
        ep = list;
        list = NEXT(list);
        NEXT(ep) = 0;

        if((*comparefunc)(ep,new_list) < 0)
        {
            NEXT(ep) = new_list;
            new_list = ep;
            continue;
        }

        pcp = new_list;
        cp = NEXT(new_list);
        while(cp){
            if((*comparefunc)(ep,cp) <= 0){
                NEXT(pcp) = ep;
                NEXT(ep) = cp;
                break;
            }
            pcp = cp;
            cp = NEXT(cp);
        }

        if(cp == 0){
            NEXT(pcp) = ep;
        }

    }

    return new_list;

}

int Kmpcharcmp(void *pat, void *ob, int a, int b)
{
    return ((char*)pat)[a] - ((char*)ob)[b];
}

int Kmpintegercmp(void *pat, void *ob, int a, int b)
{
    return ((int*)pat)[a] - ((int*)ob)[b];
}

/* Pattern string pat failure function value */
void GetFailure(void *pat, int pat_len, comparefunc cmp ,int *f)
{
    int j = 0, k = -1;
    f[0] = -1;                                       /* initial f[0] as -1 */
    while (j < pat_len - 1)
        if (k == -1 || cmp(pat,pat,k,j) == 0)
            f[++j] = ++k;
        else		                             /* pat[k] and pat[j] don't match */
            k = f[k];		                     /* Looking for a new match */
}

/* KMP Matching algorithm Search pattern 
** array of pat in the target for the 
** first time from the first p a character 
** in the string of ob began to appear */

int KMP_find(void *pat, void *ob, int pat_len, int ob_len, comparefunc cmp, int *f, int p)
{
    int i = p, j = 0;
    while (i < ob_len && j < pat_len && pat_len - j <= ob_len - i)
       if (j == -1 || cmp(pat,ob,j,i) == 0){
           i++; j++;			              /* Pattern string of pat and target string of ob the current position of the back */
       }
       else	                                      /* pat[k] and ob[j] don't match  */
           j = f[j];		                      /* Looking for a new pattern matching string of pat character position */

    if (j < pat_len)
        return -1;		                      /* Matching failure */
    else
        return i - j;			              /* Matching successful */
}

void InitGlobal(struct global *gp, struct option *popt, struct macro_define *pmac)
{
    gp->filename = popt->filename;
    gp->opt = popt;
    gp->madefine = pmac;
    gp->start = 0;
    gp->conflict_count = 0;
    gp->parse_name = 0;
    gp->token_prefix = 0;
    gp->include = 0;
    gp->token_type = 0;
    gp->token_destruct = 0;
    gp->token_destruct_line = -1;
    gp->nonterminal_type = 0;
    gp->nonterminal_destruct = 0;
    gp->nonterminal_destruct_line = -1;
    gp->code = 0;
    gp->default_code = 0;
    gp->error = 0;
    gp->failed = 0;
    gp->accept = 0;
}
