#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <assert.h>
#include "mathew.h"

#define CHECK_ACTION(a) assert(a->type == SH_RESOLVED || a->type == RD_RESOLVED || a->type == CONFLICT)

static struct action *action_freelist = 0;

/* Allocate a new parser action */
PRIVATE struct action *newaction()
{
    struct action *newap;

    if(action_freelist == 0 ){
      int i;
      int amt = 80;
      action_freelist = (struct action *)Mem_alloc( sizeof(struct action)*amt);
      MEM_CHECK(action_freelist);
      for(i = 0; i< amt-1; i++) action_freelist[i].next = &action_freelist[i+1];
      action_freelist[amt-1].next = 0;
    }
    newap = action_freelist;
    action_freelist = action_freelist->next;
    return newap;
}
/* Delete acton reback space to action_freelist */
PRIVATE void deleteaction(struct action *ap)
{
    ap->next = action_freelist;
    action_freelist = ap;
}

void Add_Action(struct action **pap, enum e_action type, struct symbol *sp, char *arg)
{
    struct action *newap;

    newap = newaction();
    newap->type = type;
    newap->forward = sp;

    if(type == SHIFT){
        newap->x.stp = (struct state *)arg;
    }else{
        newap->x.rp = (struct rule *)arg;
    }

    newap->next = *pap;
    *pap = newap;
}

PRIVATE int resolve_conflict(struct action *apx, struct action *apy)
{
    struct symbol *spx, *spy;
    int conflict_count = 0;

    assert(apx->forward == apy->forward);  /* Otherwise there would be no conflict */

    if(apx->type == SHIFT && apy->type == REDUCE ){
        spx = apx->forward;
        spy = apy->x.rp->precsymbol;
        if(spy == 0 || spx->prec < 0 || spy->prec < 0){ /* One of them is assoc = -1(not UNKNOW) */
            /* Not enough precedence information. */
            apy->type = CONFLICT;
            conflict_count++;
        }else if(spx->prec > spy->prec){    /* Lower precedence(action.type) wins */
            apy->type = RD_RESOLVED;        /* Used action SHIFT (apx) drop REDUCE (apy) */
        }else if(spx->prec < spy->prec){
            apx->type = SH_RESOLVED;
        }else if(spx->prec == spy->prec && spx->assoc == RIGHT){ /* Use operator */
            apy->type = RD_RESOLVED;                             /* associativity */
        }else if(spx->prec == spy->prec && spx->assoc == LEFT){  /* to break tie */
            apx->type = SH_RESOLVED;
        }else{
            /* Only left spx->assoc == NONASSOC by if(spy == 0 ...
            ** because if spx > 0 through test only equal NONASSOC */
            assert(spx->assoc == NONASSOC);
            apy->type = CONFLICT;
            conflict_count++;
        }
    }else if(apx->type == REDUCE && apy->type == REDUCE){
        spx = apx->x.rp->precsymbol;
        spy = apy->x.rp->precsymbol;
        /* One of them assoc is -1 can't be compare */
        if(spx == 0|| spy == 0 || spx->prec < 0 || spy->prec < 0 || spx->prec == spy->prec){
            apy->type = CONFLICT;
            conflict_count++;
        }else if(spx->prec > spy->prec){
            apy->type = RD_RESOLVED;
        }else if(spx->prec < spy->prec){
            apx->type = RD_RESOLVED;
        }
    }else{ /* Action is sort so no reduce shift conflict */
        CHECK_ACTION(apx);
        CHECK_ACTION(apy);
    }
    /* The REDUCE/SHIFT case cannot happen because SHIFTs come before
    ** REDUCEs on the list.  If we reach this point it must be because
    ** the parser conflict had already been resolved. */
    return conflict_count;
}

void BuildActions(struct global *gp)
{
    struct config *cfp;
    struct state  *stp;
    struct symbol *sp;
    struct rule   *rp;
    struct action *ap,*nap;
    char *str;
    int i;

    /* Add reduce action */
    stp = gp->states;
    while(stp){
        cfp = stp->cfp;
        while(cfp){
            if(cfp->rp->nrhs == cfp->dot){        /* Is dot at extreme right? */
                sp = gp->symbols;                 /* Sorted symbols */
                while(sp && (sp->index < gp->terminal_num)){
                    if(BITISSET(cfp->followset.value,sp->index)){
                        Add_Action(&stp->ap,REDUCE,sp,(char *)cfp->rp);
                    }
                    sp = sp->next;
                }
            }
            cfp = cfp->next;
        }
        stp = stp->next;
    }
    /* Add accept action configuration compare c1->rp->index - c2->rp->index */
    Add_Action(&gp->states->ap,ACCEPT,gp->start_symbol,0);
    /* Resolve conflicts */
    stp = gp->states;
    while(stp){
        stp->ap = (struct action *)Insertsort((char *)stp->ap,(char **)&stp->ap->next,Actioncmp);
        for(ap = stp->ap; ap && ap->next; ap = ap->next){
          for(nap = ap->next; nap && nap->forward == ap->forward; nap = nap->next){
             /* The two actions "ap" and "nap" have the same lookahead.
             ** Figure out which one should be used */
             gp->conflict_count += resolve_conflict(ap,nap);
          }
        }
        stp = stp->next;
    }

    /* shift reduce conflict choose shiift.
    ** Report an error for each rule that can never be reduced. */
    rp = gp->rule;
    while(rp){
        rp->canReduce = FALSE;
        rp = rp->next;
    }

    stp = gp->states;
    while(stp){
        ap = stp->ap;
        while(ap){
            if( ap->type == REDUCE) ap->x.rp->canReduce = TRUE; /* In several choose first come rule reduce */
            ap = ap->next;
        }
        stp = stp->next;
    }

    for(rp = gp->rule; rp; rp = rp->next){
        if(rp->canReduce || rp->used == FALSE) continue;
        Error_fprintf(THIS_RULE_CANNOT_BE_REDUCED);
        fprintf(GetErrorStream(),"%s -> ",rp->lhs->name);
        for(i = 0; i < rp->nrhs; i++){
            fprintf(GetErrorStream(),"%s ",rp->rhs[i]->name);
        }
        fprintf(GetErrorStream(),".\n");

        gp->err_count++;
    }
    Output_fprintf(CONFLICT_COUNT,gp->conflict_count);
}

void CompressActions(struct global *gp)
{
    struct state *stp;
    struct action *ap, *ap2, *preap, *nextpap, *finalap;
    struct rule *rp, *rp2, *rbest;
    int nbest, n;

    for(stp = gp->states; stp; stp = stp->next){
        nbest = 0;
        rbest = 0;
        for(ap = stp->ap; ap; ap = ap->next){
            if(ap->type != REDUCE) continue;
                rp = ap->x.rp;
            if(rp == rbest) continue;
            n = 1;
            for(ap2 = ap->next; ap2; ap2 = ap2->next){
                if(ap2->type != REDUCE) continue;
                rp2 = ap2->x.rp;
                if(rp2 == rp) n++;
            }
            if(n > nbest){
                nbest = n;
                rbest = rp;
            }
        }
        /* Do not make a default if the number of rules to default
        ** is not at least 2 */
        if(nbest < 2) continue;

        /* Combine matching REDUCE actions into a single default */
        preap = 0;
        ap = stp->ap;
        while(ap){
           if(ap->type == REDUCE && ap->x.rp == rbest) break; /* find first best */
           preap = ap;
           ap = ap->next;
        }

        finalap = ap;
        ap = ap->next;
        finalap->forward = Symbol_new("[default]",NONTERMINAL);
        finalap->next = 0;

        /* Find first not best reduce */
        if(preap == 0){
            while(ap){
                if(ap->type != REDUCE || ap->x.rp != rbest)break;
                nextpap = ap->next;
                deleteaction(ap);
                ap = nextpap;
            }
            if(ap == 0)continue;
            stp->ap = ap;  /* Find a new action head */
            preap = ap;    /* Set not best be pre */
            ap = ap->next;
        }else{
            preap->next = ap; /* Jump first best reduce */
        }

        /* Remove all most reduce rule actions */
        while(ap){
           if(ap->type == REDUCE && ap->x.rp == rbest) {
               preap->next = ap->next;      /* Jump link final action pointer */
               deleteaction(ap);
               ap = preap;
           }
           preap = ap;
           ap = ap->next;
        }
        preap->next = finalap;
    }
    
}

/* Generate a report of the paser (the "y.out" file) */
void GenerateOutputReport(struct global *gp)
{
    struct state  *stp;
    struct config *cfp;
    struct action *ap;
    FILE *fp;
    char *outfile;
    char buf[20];

    outfile = Make_filename(gp->filename,".out");
    fp = fopen(outfile,"wb");
    if(fp == 0){
        Error_fprintf(CANNOT_OPEN_FILE,outfile);
        return;
    }
    stp = gp->states;
    while(stp){
        fprintf(fp,"State %d:\r\n",stp->index);
        cfp = gp->opt->basisflag == 1 ? stp->bp : stp->cfp;
        while(cfp){
            if(cfp->dot == cfp->rp->nrhs){
                sprintf(buf,"(%d)",cfp->rp->index);
                fprintf(fp,"%16s",buf);
            }else{
                fprintf(fp,"%16s","");
            }
            ConfigPrint(fp,cfp);
            fprintf(fp," ");
            SetPrint(fp,cfp->followset,gp);
            fprintf(fp,"\r\n");
            PlinkPrint(fp,cfp->fplp,"To ");
            PlinkPrint(fp,cfp->bplp,"From ");
            cfp = gp->opt->basisflag == 1 ? cfp->bp : cfp->next;
        }
        fprintf(fp,"\r\n");
        ap = stp->ap;
        while(ap){
            if(ActionPrint(fp,ap,36)){
                fprintf(fp,"\r\n");
            }
            ap = ap->next;
        }
        fprintf(fp,"\r\n");
        stp = stp->next;
    }
    fclose(fp);
}
