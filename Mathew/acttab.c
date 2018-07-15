#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <assert.h>
#include "mathew.h"

PRIVATE int compute_action(struct global *gp, struct action *ap)
{
    int act;
    if(ap->type == SHIFT){
        act = ap->x.stp->index;
    }else if(ap->type == REDUCE){
        act = ap->x.rp->index + gp->state_num;
    }else if(ap->type == ACCEPT){
        act = gp->state_num + gp->rule_num;
    }else{
        act = -1;
    }
    return act;
}

PRIVATE void acttab_action(struct alookahead *alp, int lookahead, int action)
{
    if(alp->nLookahead >= alp->nLookaheadAlloc){
        alp->nLookaheadAlloc += 30;
        alp->aLookahead = Mem_relloc(alp->aLookahead,sizeof(alp->aLookahead[0])*alp->nLookaheadAlloc);
        MEM_CHECK(alp->aLookahead);
    }
    if(alp->nLookahead == 0){
        alp->minLookahead = lookahead;
        alp->maxLookahead = lookahead;
    }else{
        if(alp->minLookahead > lookahead){
            alp->minLookahead = lookahead;
        }
        if(alp->maxLookahead < lookahead){
            alp->maxLookahead = lookahead;
        }
    }
    alp->aLookahead[alp->nLookahead].lookahead = lookahead;
    alp->aLookahead[alp->nLookahead].action = action;
    alp->nLookahead++;
}


PRIVATE int acttab_insert(struct acttab *pActtab, struct alookahead *alp, int state)
{
    int i, j, k, n;

    if(alp->nLookahead <= 0)
        return NO_OFFSET;

    n = alp->maxLookahead + 1;
    /* confirm enough space */
    if(pActtab->nAction + n >= pActtab->nActionAlloc){
        int oldAlloc = pActtab->nActionAlloc;
        pActtab->nActionAlloc = pActtab->nAction + n + pActtab->nActionAlloc + 30;
        pActtab->aAction = Mem_relloc(pActtab->aAction,sizeof(pActtab->aAction[0])*pActtab->nActionAlloc);
        MEM_CHECK(pActtab->aAction);
        /* Init append action table */
        for(i = oldAlloc; i < pActtab->nActionAlloc; i++){
            pActtab->aAction[i].state = -1;
            pActtab->aAction[i].action = -1;
        }
    }

    /* comb insert */
    for(i = 0; i < pActtab->nAction + alp->minLookahead; i++){
        if(pActtab->aAction[i].state < 0){ /* if we find comb anadontia then we mark it */
            for(j = 0; j < alp->nLookahead; j++){
                k = alp->aLookahead[j].lookahead - alp->minLookahead + i;
                if(pActtab->aAction[k].state >= 0)break;
            }
            if(j < alp->nLookahead)continue;
            break;
        }
    }

    for(j = 0; j < alp->nLookahead; j++){
        k = alp->aLookahead[j].lookahead - alp->minLookahead + i;
        pActtab->aAction[k].action = alp->aLookahead[j].action;
        pActtab->aAction[k].state = state;
        if(k >= pActtab->nAction)pActtab->nAction = k + 1;
    }

    alp->nLookahead = 0;
    return i - alp->minLookahead;

}

void Initacttab(struct acttab *pActtab)
{
    memset(pActtab,0,sizeof(*pActtab));
}

void Releaseacttab(struct acttab *pActtab)
{
    Mem_free(pActtab->aAction);
    Mem_free(pActtab->al[0].aLookahead);
    Mem_free(pActtab->al[1].aLookahead);
    memset(pActtab,0,sizeof(*pActtab));
}

void ComputeActtab(struct global *gp, struct acttab *pActtab)
{
    int act;
    struct action *ap;
    struct state *stp;
    for(stp = gp->states; stp; stp = stp->next){
        stp->defaultAction = gp->state_num + gp->rule_num + 1; /* error action */
        for(ap = stp->ap; ap; ap = ap->next){
            act = compute_action(gp,ap);
            if(act < 0)continue;
            if(ap->forward->index < gp->terminal_num){
                acttab_action(&pActtab->al[0],ap->forward->index,act);
            }else if(ap->forward->index < gp->symbol_num){ /* ap->forward->index >= gp->terminal_num && ap->forward->index < gp->symbol_num */
                acttab_action(&pActtab->al[1],ap->forward->index,act);
            }else{
                stp->defaultAction = act;    /* [default] action */
            }
        }
        /* if alp->nLookahead <= 0 set offset NO_OFFSET */
        stp->tokOffset = acttab_insert(pActtab,&pActtab->al[0],stp->index);
        if(stp->tokOffset != NO_OFFSET){
            if(stp->tokOffset < pActtab->minTokOffset)pActtab->minTokOffset = stp->tokOffset;
            if(stp->tokOffset > pActtab->maxTokOffset)pActtab->maxTokOffset = stp->tokOffset;
        }

        stp->ntOffset = acttab_insert(pActtab,&pActtab->al[1],stp->index);
        if(stp->ntOffset != NO_OFFSET){
            if(stp->ntOffset < pActtab->minNtOffset)pActtab->minNtOffset = stp->ntOffset;
            if(stp->ntOffset > pActtab->maxNtOffset)pActtab->maxNtOffset = stp->ntOffset;
        }
    }
}
