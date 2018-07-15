#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>
#include "mathew.h"

void SetNew(struct set *s, int n)
{
    int i;
    int size;
    SEG *set;

    size = SEGSIZE(n);
    set = (SEG *)Mem_alloc(BITS_PER_SEG * size);
    MEM_CHECK(set);

    for(i = 0; i < size; i++){
        set[i] = 0;
    }

    s->size = size;
    s->value = set;
}

void SetResize(struct set *s, int n)
{
    int size;

    size = SEGSIZE(n);
    s->value = Mem_relloc(s->value,size);

    while(s->size < size){
        s->value[s->size++] = 0; /* Make sure append item clearly */
    }
}

void SetAdd(struct set *s, int e)
{
    SETBIT(s->value,e);
}

void SetUnion(struct set *s1, struct set s2)
{
    int i;
    int size = s1->size < s2.size ? s1->size : s2.size; /* Union minimum size */

    for(i = 0; i < size; i++){
        (s1->value)[i] |= (s2.value)[i];
    }
}

/* No in SetUnion because for save other func used SetUnion performance */
int SetIsExpand(struct set s1, struct set s2)
{
    int i;
    int size;

    if(s1.size != s2.size) return 1;

    size = s1.size;
    for(i = 0; i < size; i++){
        if(((s1.value)[i] | (s2.value)[i]) ^ (s1.value)[i]) return 1;
    }
    return 0;
}

void SetFree(struct set *s)
{
    s->size = 0;
    Mem_free(s->value);
}
