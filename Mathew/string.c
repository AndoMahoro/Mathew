#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "mathew.h"

void Tplstring_init(struct TplString *pstr, int initsize)
{
    pstr->data = (char*)Mem_alloc(sizeof(char)*initsize);
    MEM_CHECK(pstr->data);
    pstr->size = 0;
    pstr->capacity = initsize;

}

void Tplstring_reset(struct TplString *pstr)
{
    pstr->size = 0;
}

void Tplstring_free(struct TplString *pstr)
{
    if(pstr->data)
        Mem_free(pstr->data);
    pstr->size = 0;
    pstr->capacity = 0;
}

void Tplstring_realloc(struct TplString *pstr, int new_size)
{
    int capacity;

    capacity = pstr->capacity;
    while(capacity < new_size)
        capacity *= 2;
    pstr->data = (char*)Mem_relloc(pstr->data,capacity);
    MEM_CHECK(pstr->data);
    pstr->capacity = capacity;

}

void Tplstring_chcat(struct TplString *pstr, int ch)
{
    int size;
    size = pstr->size + 1;
    if(size > pstr->capacity)
        Tplstring_realloc(pstr,size);
    ((char*)pstr->data)[pstr->size] = ch;
    pstr->size = size;
}
