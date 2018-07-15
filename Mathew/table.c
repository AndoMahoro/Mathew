#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include "mathew.h"


static struct table *str_tbl;
static struct table *symbol_tbl;
static struct table *state_tbl;
static struct table *config_tbl;
/* static struct table *map_tbl; */

/* Initialize the hash table*/
PRIVATE void basic_init(struct table **tbl/* struct tble * &tbl */, int size){
  if(*tbl) return;
  *tbl = (struct table*)Mem_alloc( sizeof(struct table));
  if( *tbl ){
    (*tbl)->size = size;
    (*tbl)->count = 0;
    (*tbl)->tbl = (struct node*)Mem_alloc(
      (sizeof(struct node) + sizeof(struct node*))*size ); /* Alloc node and hash table lookups memory*/

    MEM_CHECK(*tbl)else{
      int i;
      (*tbl)->ht = (struct node**)&((*tbl)->tbl[size]);
      for(i=0; i<size; i++) (*tbl)->ht[i] = 0;
    }
  }
}

/* String hash function*/
PRIVATE unsigned int strhash(char *str,unsigned int N)
{
    /* SDBMHash */
    unsigned int hash = 0;
    while (*str)
    {
        // equivalent to: hash = 65599*hash + (*str++);
        hash = (*str++) + (hash << 6) + (hash << 16) - hash;
    }
    return hash & N;
}

/* Symbol hash function*/
PRIVATE unsigned int symbolhash(struct symbol *a,unsigned int N)
{
    return strhash(a->name,N);
}

/* Config hash function*/
PRIVATE unsigned int confighash(struct config *a,unsigned int N)
{
    unsigned int hash = 0;

    hash = hash*571 + a->rp->index*37 + a->dot;
    return hash & N;
}

/* State hash function*/
PRIVATE unsigned int statehash(struct state *a,unsigned int N)
{
    unsigned int hash = 0;
    struct config *c = a->bp;

    while( c ){
        hash = hash*571 + c->rp->index*37 + c->dot;
        c = c->bp;
    }
    return hash & N;
}


PRIVATE char *basic_find(struct table *tbl, char *key,
                         hashfunc hash,
                         comparefunc cmp)
{
     unsigned int hx;
     struct node *np;

     if(tbl == 0)return 0;
     hx = hash(key,tbl->size - 1);
     np = tbl->ht[hx];

     while(np){
         if(cmp(np->data,key)== 0)break;
         np = np->next;
     }

     return np ? np->data : 0;
}

PRIVATE int basic_insert(struct table *tbl,char *data,
                         hashfunc hash,
                         comparefunc cmp)
{
     unsigned int hx;
     struct node *np;

     if(tbl == 0)return 0;
     hx = hash(data,tbl->size - 1);
     np = tbl->ht[hx];

     while(np){
         if(cmp(np->data,data)== 0)return 0; /* Existing entry with the same key is found */
         np = np->next;
     }

     if(tbl->count >= tbl->size){
         /* Need to expand capacity */
         int i;
         struct table newtbl;
         newtbl.size = tbl->size * 2;
         newtbl.count = tbl->count;
         newtbl.tbl = (struct node*)Mem_alloc((sizeof(struct node)+sizeof(struct node*))*newtbl.size);
         MEM_CHECK(newtbl.tbl);
         newtbl.ht = (struct node**)&(newtbl.tbl[newtbl.size]);

         for(i = 0; i < newtbl.size; i++)newtbl.ht[i] = 0;
         for(i = 0; i < newtbl.count; i++){
             /* To add the content of the old new hash table */
             struct node *oldnp, *newnp;
             oldnp = &(tbl->tbl[i]);
             hx = hash(oldnp->data, newtbl.size - 1);
             newnp = &(newtbl.tbl[i]);
             newnp->data = oldnp->data;
             newnp->next = newtbl.ht[hx];
             newtbl.ht[hx] = newnp;
         }
         Mem_free(tbl->tbl);
         *tbl = newtbl; /* Overwrite the old info */
         hx = hash(data,tbl->size - 1);
     }

     /* Insert the new data */
     np = &(tbl->tbl[tbl->count++]);
     np->data = data;
     np->next = tbl->ht[hx];
     tbl->ht[hx] = np;

     return 1;
}

void Table_Init(void)
{
    basic_init(&str_tbl,1024);
    basic_init(&symbol_tbl,128);
    basic_init(&state_tbl,128);
    basic_init(&config_tbl,64);
}

/* Find and insert function*/
char *Str_find(char *str)
{
    return basic_find(str_tbl,(char*)str, (unsigned int(*)())strhash, (int(*)())strcmp);
}
struct symbol *Symbol_find(char *x)
{
    return (struct symbol*)basic_find(symbol_tbl, (char*)x, (unsigned int(*)())strhash, (int(*)())Symbolcmp_1);
}
struct state *State_find(struct state *x)
{
    return (struct state*)basic_find(state_tbl, (char*)x, (unsigned int(*)())statehash, (int(*)())Statecmp);
}
struct config *Config_find(struct config *x)
{
    return (struct config*)basic_find(config_tbl, (char*)x, (unsigned int(*)())confighash, (int(*)())Configcmp);
}

int Str_insert(char *str)
{
    return basic_insert(str_tbl,str, (unsigned int(*)())strhash, (int(*)())strcmp);
}
int Symbol_insert(struct symbol *x)
{
    return basic_insert(symbol_tbl,(char*)x, (unsigned int(*)())symbolhash, (int(*)())Symbolcmp_2);
}
int State_insert(struct state *x)
{
    return basic_insert(state_tbl,(char*)x, (unsigned int(*)())statehash, (int(*)())Statecmp);
}
int Config_insert(struct config *x)
{
    return basic_insert(config_tbl,(char*)x, (unsigned int(*)())confighash, (int(*)())Configcmp);
}

int Symbol_count(void)
{
    return symbol_tbl ? symbol_tbl->count : 0;
}

int Str_count(void)
{
    return str_tbl ? str_tbl->count : 0;
}

int State_count(void)
{
    return state_tbl ? state_tbl->count : 0;
}

int Config_count(void)
{
    return config_tbl ? config_tbl->count : 0;
}

void Config_clear(int (*freefunc)())
{
    int i;
    if(config_tbl == 0 || config_tbl->count == 0)return;
    if(freefunc){
        for(i = 0; i < config_tbl->count; i++){
            (*freefunc)(config_tbl->tbl[i].data);
        }
    }
    for(i = 0; i < config_tbl->size; i++){
        config_tbl->ht[i] = 0;
    }
    config_tbl->count = 0;
    
}
/* Test hash table*/
void TestStrHashTable(void)
{
    int i;
    struct node *np;
    printf("String hash table test:\n");
    for(i = 0; i<str_tbl->size; i++){
        if(str_tbl->ht[i]!= 0){
            printf("str_tbl->ht[%d]\t->",i);
            np = str_tbl->ht[i];
            while(np){
                printf("%s ",np->data);
                np = np->next;
            }
            printf("\n");
        }
    }
}

void TestSymbolHashTable(void)
{
    int i;
    struct node *np;
    printf("Symbol hash table test:\n");
    for(i = 0; i<symbol_tbl->size; i++){
        if(symbol_tbl->ht[i]!= 0){
            printf("symbol_tbl->ht[%d]\t->",i);
            np = symbol_tbl->ht[i];
            while(np){
                printf("%s ",((struct symbol*)np->data)->name);
                np = np->next;
            }
            printf("\n");
        }
    }
}
