#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>
#include "mathew.h"


/* Recursion compute Epsilon */
PRIVATE int computeEpsilon(struct global *gp, struct symbol *sp)
{
   int i;
   int epsilon = 0;
   struct rule *rp = sp->rule; /* Symbol link rule is rp->lhs->rule = rp; */

   /* Add default rule to no rule symbol promise rp not empty */
   if(sp->type == NONTERMINAL && rp == 0){
       rp = ProcessNoRuleSymbol(gp, sp);
       Error_fprintf(NONTERMINAL_IMPLICIT_RULE,sp->name,sp->name);
   }
   if(sp->epsilon == TRUE) return 1;
   
   while(rp){
       for(i = 0; i < rp->nrhs; i++){
           if(sp == rp->rhs[i]){
               epsilon = FALSE; /* This avoid else if block epsilon = 1 next time sp == rp->rhs will return 1 */
               break;
           }else if(rp->rhs[i]->type == NONTERMINAL){
               epsilon = computeEpsilon(gp,rp->rhs[i]);
               if(epsilon == FALSE) break;
           }else{
               epsilon = FALSE; /* This avoid else if block epsilon = 1 next time sp == rp->rhs will return 1 */
               break; 
           }
       }

       if(i == rp->nrhs)
       {
           sp->epsilon = epsilon = TRUE;
           break;
       }else{
           /* Compute left nonterminal epsilon */
           for(i++; i < rp->nrhs; i++){
               if(sp == rp->rhs[i]){
                   continue;
               }else if(rp->rhs[i]->type == NONTERMINAL){
                   computeEpsilon(gp,rp->rhs[i]);
               }
           }
       }
       rp = rp->nextlhs;
   }
   return epsilon; /* sp->rule == 0 epsilon == 1 */
}

/* Recursion first sets */
PRIVATE void computeFirstSets(struct global *gp,struct symbol *sp)
{
    int i;
    struct rule *rp = sp->rule;

    if(sp->firstset.size == 0)
        SetNew(&sp->firstset,gp->terminal_num);

    /* Add default rule to no rule symbol promise rp not empty */
    if(sp->type == NONTERMINAL && rp == 0){
        rp = ProcessNoRuleSymbol(gp, sp);
        Error_fprintf(NONTERMINAL_IMPLICIT_RULE,sp->name,sp->name);
    }
    while(rp){
        rp->used = TRUE;                  /* Rule is used eliminate unused production */
        for(i = 0; i < rp->nrhs; i++){
            if(sp == rp->rhs[i]){
                if(sp->epsilon == FALSE) break;
            }else if(rp->rhs[i]->type == NONTERMINAL){
                computeFirstSets(gp,rp->rhs[i]);
                SetUnion(&sp->firstset,rp->rhs[i]->firstset);
                if(rp->rhs[i]->epsilon == FALSE) break;
            }else{
                SetAdd(&sp->firstset,rp->rhs[i]->index);
                break;
            }
        }

        /* Compute left nonterminal first set */
        for(i++; i < rp->nrhs; i++){
            if(sp == rp->rhs[i]){
                continue;
            }else if(rp->rhs[i]->type == NONTERMINAL){
                computeFirstSets(gp,rp->rhs[i]);
            }
        }

        rp = rp->nextlhs;
    }
}

void BuildFirstSets(struct global *gp)
{
    if(gp->terminal_num == 0)return;

    /* Computer start symbol Epsilon */
    computeEpsilon(gp,gp->start_symbol);
   
    /* Computer start symbol first sets and set rule used */
    computeFirstSets(gp,gp->start_symbol);

}

void BuildFollowSets(struct global *gp)
{
    struct state  *stp0,*stp;
    struct config *cfp;
    struct plink  *plp;
    int progress;

    /* Calculation states follow set */
    do{
        stp = gp->states;
        progress = 0;
        while(stp){
            for(cfp = stp->cfp; cfp; cfp = cfp->next){
                if(cfp->status == COMPLETE)continue;
                plp = cfp->fplp;
                while(plp){
                    if(SetIsExpand(plp->cfp->followset,cfp->followset)){
                        SetUnion(&plp->cfp->followset,cfp->followset);
                        plp->cfp->status = INCOMPLETE;
                        progress = 1;
                    }
                    plp = plp->next;
                }
                cfp->status = COMPLETE;
            }
            stp = stp->next;
        }
    }while(progress);

}
