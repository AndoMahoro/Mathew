#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <assert.h>
#include "mathew.h"

#define CONFIGLIST_BASIS    configlist_return(&basis)   /* Return basis and reset basis */
#define CONFIGLIST_CURRENT  configlist_return(&current) /* Return current and reset current */

static struct plink  *plink_freelist;
static struct config *config_freelist;
static struct state  *state_freelist;
static struct state  *state_list;
static struct state  **state_listend;

static struct config *basis;               /* Basis config set */
static struct config *current;             /* Config Specifications set */
static struct config **currentend;

PRIVATE struct state *buildstate(struct global *gp);

PRIVATE void configlist_init()
{
    plink_freelist  = 0;
    config_freelist = 0;
    state_freelist  = 0;
    state_list      = 0;
    basis           = 0;
    current         = 0;
    currentend      = &current;
    state_listend   = &state_list;
}

/* Allocate a new plink */
PRIVATE struct plink *newplink()
{
    struct plink *newplp;

    if(plink_freelist == 0){
        int i;
        int amt = 80;
        plink_freelist = (struct plink *)Mem_alloc( sizeof(struct plink)*amt);
        MEM_CHECK(plink_freelist);
        for(i = 0; i < amt-1; i++){
            plink_freelist[i].next = &plink_freelist[i+1];
        }
        plink_freelist[amt-1].next = 0;
    }

    newplp = plink_freelist;
    plink_freelist = plink_freelist->next;

    return newplp;
}

/* Return a pointer to a new configuration */
PRIVATE struct config *newconfig()
{
    struct config *newcfp;

    if(config_freelist == 0 ){
        int i;
        int amt = 8;
        config_freelist = (struct config *)Mem_alloc( sizeof(struct config)*amt);
        MEM_CHECK(config_freelist);
        for(i = 0; i < amt-1; i++){
            config_freelist[i].next = &config_freelist[i+1];
        }
        config_freelist[amt-1].next = 0;
    }

    newcfp = config_freelist;
    config_freelist = config_freelist->next;

    return newcfp;
}

PRIVATE struct state *newstate()
{
    struct state *newstp;

    if(state_freelist == 0 ){
        int i;
        int amt = 4;
        state_freelist = (struct state *)Mem_alloc( sizeof(struct state)*amt);
        MEM_CHECK(state_freelist);
        for(i = 0; i < amt-1; i++){
            state_freelist[i].next = &state_freelist[i+1];
        }
        state_freelist[amt-1].next = 0;
    }

    newstp = state_freelist;
    state_freelist = state_freelist->next;

    return newstp;
}

/* Add a plink to a plink list */
PRIVATE void plink_add(struct plink **plpp, struct config *cfp)
{
    struct plink *newplp;
    newplp = newplink();
    newplp->next = *plpp;
    newplp->cfp = cfp;
    *plpp = newplp;
}

/* Transfer every plink on the list "from" to the list "to" head */
PRIVATE void plink_copy(struct plink **to, struct plink *from)
{
    struct plink *preplp;

    if(from == 0)return;

    while(from){
        preplp = from;
        from = from->next;
    }

    preplp->next = *to;
    *to = preplp;
}

/* Delete every plink on the list */
PRIVATE void plink_delete(struct plink *plp)
{
    struct plink *nextplp;

    while(plp){
        nextplp = plp->next;
        plp->next = plink_freelist;
        plink_freelist = plp;
        plp = nextplp;
    }
}

/* There have tow same core state we need merge them look ahead symbol(this say follow set) */
PRIVATE void merge_concentric(struct state *target, struct state *source)
{
    struct config *a,*b;

    a = source->bp;
    b = target->bp;
    while(a && b){
        plink_copy(&b->bplp,a->bplp);
        plink_delete(a->fplp);
        a->fplp = a->bplp = 0;
        a = a->bp;
        b = b->bp;
    }

}

/* The configuration return to config_freelist */
PRIVATE void deleteconfig(struct config *cfp)
{
    cfp->next = config_freelist;
    config_freelist = cfp;
}

PRIVATE struct config *configlist_return(struct config **pcfp)
{
    struct config *old;
    old = *pcfp;
    *pcfp = 0;
    return old;
}

PRIVATE void configlist_erase(struct config *cfp)
{
    struct config *nextcfp;
    while(cfp){
        nextcfp = cfp->next;
        assert(cfp->fplp == 0);
        assert(cfp->bplp == 0);
        if(cfp->followset.size){
            SetFree(&cfp->followset);
        }
        deleteconfig(cfp);
        cfp = nextcfp;
    }
}

PRIVATE void configlist_reset()
{
    basis      = 0;
    current    = 0;
    currentend = &current;
    Config_clear(0);
}

PRIVATE struct config *configlist_addbasis(struct global *gp, struct rule *rp, int dot)
{
    struct config *cfp, temp;

    assert(currentend != 0);
    temp.rp = rp;
    temp.dot = dot;

    cfp = Config_find(&temp);
    if(cfp == 0){
        cfp = newconfig();
        cfp->rp = rp;
        cfp->dot = dot;
        SetNew(&cfp->followset,gp->terminal_num);
        cfp->stp = 0;
        cfp->fplp = cfp->bplp = 0;
        cfp->next = 0;
        cfp->bp = 0;
        *currentend = cfp;
        currentend = &cfp->next;
        Insertitem((char**)&basis,(char**)&basis->bp,(char*)cfp,Configcmp);
        Config_insert(cfp);
    }
    return cfp;
}

/* Add another configuration to the configuration list */
PRIVATE struct config *configlist_add(struct global *gp, struct rule *rp, int dot)
{
    struct config *cfp, temp;

    assert(currentend != 0);
    temp.rp = rp;
    temp.dot = dot;
    cfp = Config_find(&temp);
    if(cfp == 0){
        cfp = newconfig();
        cfp->rp = rp;
        cfp->dot = dot;
        SetNew(&cfp->followset,gp->terminal_num);
        cfp->stp = 0;
        cfp->fplp = cfp->bplp = 0;
        cfp->next = 0;
        cfp->bp = 0;
        *currentend = cfp;
        currentend = &cfp->next;
        Config_insert(cfp);
    }
    return cfp;
}
PRIVATE void configlist_closure(struct global *gp, struct config *ccfp)
{
    struct config *cfp, *newcfp;
    struct rule   *rp, *newrp;
    struct symbol *sp, *xsp;
    int i, dot;

    assert(currentend != 0);

    for(cfp = ccfp; cfp; cfp = cfp->next){
        rp = cfp->rp;
        dot = cfp->dot;
        if(dot >= rp->nrhs) continue;
        sp = rp->rhs[dot];
        if( sp->type == NONTERMINAL ){
            for(newrp = sp->rule; newrp; newrp = newrp->nextlhs){
                newcfp = configlist_add(gp,newrp,0);
                /* Same left hand symbol rule add look ahead symbols (follow set) */
                for(i = dot+1; i < rp->nrhs; i++){
                    xsp = rp->rhs[i];
                    if( xsp->type == TERMINAL ){
                        SetAdd(&newcfp->followset,xsp->index);
                        break;
                    }else{
                        SetUnion(&newcfp->followset,xsp->firstset);
                        if( xsp->epsilon == FALSE ) break;
                    }
                }
                if(i == rp->nrhs) plink_add(&cfp->fplp,newcfp);
            }
        }
    }

}

/* The state from which successors are computed */
PRIVATE void buildshifts(struct global *gp, struct state *stp)
{
    struct config *cfp,*bcfp,*newcfp;
    struct symbol *sp, *bsp;
    struct state  *newstp;                          /* A pointer to a successor state */

    cfp = stp->cfp;
    while(cfp){
        cfp->status = INCOMPLETE;
        cfp = cfp->next;
    }

    /* Loop through all configurations of the state "stp" */
    for(cfp = stp->cfp; cfp; cfp = cfp->next){
        if(cfp->status == COMPLETE) continue;         /* Already used by inner loop */
        if(cfp->dot >= cfp->rp->nrhs) continue;       /* Can't shift this config */
        configlist_reset();                           /* Reset the new config set */
        sp = cfp->rp->rhs[cfp->dot];                  /* Symbol after the dot */
        for(bcfp = cfp; bcfp; bcfp = bcfp->next){
            if(bcfp->status == COMPLETE) continue;    /* Already used */
            if(bcfp->dot >= bcfp->rp->nrhs) continue; /* Can't shift this one */
            bsp = bcfp->rp->rhs[bcfp->dot];           /* Get symbol after dot */
            if(bsp != sp) continue;                   /* Must be same as for "cfp" */
            bcfp->status = COMPLETE;                  /* Mark this config as used */
            newcfp = configlist_addbasis(gp,bcfp->rp,bcfp->dot+1);
            plink_add(&newcfp->bplp,bcfp);
        }

        newstp = buildstate(gp);
        Add_Action(&stp->ap,SHIFT,sp,(char *)newstp);
    }

}

PRIVATE struct state *buildstate(struct global *gp)
{
    struct config *cfp;
    struct state *stp,temp;

    temp.bp  = CONFIGLIST_BASIS;       /* Basis is short by Insertitem */
    /* Get a state with the same basis */
    stp = State_find(&temp);
    if(stp){
        /* A state with the same basis already exists!  Copy all the follow-set
        ** propagation links from the state under construction into the
        ** preexisting state, then return a pointer to the preexisting state */
        merge_concentric(stp,&temp);   /* Merge concentric set */
        cfp = CONFIGLIST_CURRENT;
        configlist_erase(cfp);
    }else{
        configlist_closure(gp,current);      /* Compute the configuration closure */
        cfp = CONFIGLIST_CURRENT;
        cfp = (struct config*)Insertsort((char*)cfp,(char**)&cfp->next,Configcmp);/* Current need sort */
        stp = newstate();              /* A new state structure */
        stp->bp = temp.bp;             /* Remember the configuration basis */
        stp->cfp = cfp;                /* Remember the configuration closure */
        stp->index = gp->state_num++; /* Every state gets a sequence number */
        stp->ap = 0;                   /* No actions, yet. */
        stp->next = 0;
        *state_listend = stp;
        state_listend = &stp->next;    /* Pull a state list by build orderly index */
        State_insert(stp);             /* Add to the state table */
        buildshifts(gp,stp);         /* Recursively compute successor states */

    }
    return stp;
}

struct state *GetStates()
{
    return state_list;
}

void BuildLinks_And_ResetStatus(struct global *gp)
{
    struct config *cfp,*mother;
    struct state *stp;
    struct plink *plp;

    /* Configuration link a pointer back to the belong state
    ** convert all backlinks into forward links only forward
    ** next state look follow set inherit from the pre states */
    stp = gp->states;
    while(stp){
        cfp = stp->cfp;
        while(cfp){
            cfp->stp = stp;
            plp = cfp->bplp;
            while(plp){
                mother = plp->cfp;
                plink_add(&mother->fplp,cfp);
                plp = plp->next;
            }
            cfp->status = INCOMPLETE;
            cfp = cfp->next;
        }
        stp = stp->next;
    }

}

void BuildStates(struct global *gp)
{
    struct rule *rp;
    struct config *newcfp;

    gp->state_num = 0;
    configlist_init();

    rp = gp->start_symbol->rule;
    newcfp = configlist_addbasis(gp,rp,0);
    SetAdd(&newcfp->followset,0);          /* Add end of input '#' */
    buildstate(gp);

}

