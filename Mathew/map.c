#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include "mathew.h"

#ifndef NULL  
#define NULL 0  
#endif

PRIVATE struct RBTNode *uncleNode(struct RBTNode *node)
{
    return node->parent == node->parent->parent->lchild ? node->parent->parent->rchild : node->parent->parent->lchild;
}

PRIVATE struct RBTNode *brotherNode(struct RBTNode *node)
{
    return node == node->parent->lchild ? node->parent->rchild : node->parent->lchild;
}

/* right rotate */
/*-----------------------------------------------------------
|       node            left
|       / \             / \
|    left  z   ==>    x    node
|   / \                    / \
|  x   y                  y   z  //right rotate
-----------------------------------------------------------*/
PRIVATE void  rotateRight(struct map *mp, struct RBTNode *node)
{
    struct RBTNode *lchild = node->lchild;

    node->lchild = lchild->rchild;
    node->lchild->parent = node;

    lchild->parent = node->parent;
    if(node->parent == NULL)
        mp->root = lchild;
    else if(node->parent->rchild == node)
        node->parent->rchild = lchild;
    else
        node->parent->lchild = lchild;

    node->parent = lchild;
    lchild->rchild  = node;
}

/* left rotate */
/*-----------------------------------------------------------
|   node               right
|   / \                / \
|   x  right   ==>    node  z
|       / \           / \
|       y  z         x   y    //left rotate
-----------------------------------------------------------*/
PRIVATE void  rotateLeft(struct map *mp, struct RBTNode *node)
{
    struct RBTNode *rchild = node->rchild;

    node->rchild = rchild->lchild;
    node->rchild->parent = node;

    rchild->parent = node->parent;
    if(node->parent == NULL)
        mp->root = rchild;
    else if(node->parent->rchild == node)
        node->parent->rchild = rchild;
    else
        node->parent->lchild = rchild;

    node->parent = rchild;
    rchild->lchild  = node;
}

/* CLRS insert fixup fixed part root is black */
PRIVATE void insertFixup(struct map *mp, struct RBTNode *node)
{
    if(node->parent == NULL){
        node->color = BLACK;               /* root black */
        return;
    }

    if(node->parent->color == BLACK)       /* parent is black */
        return;

    struct RBTNode *tmp = uncleNode(node); /* parent red also have grandparent*/
    if(tmp->color == RED){
        tmp->color = BLACK;
        node->parent->color = BLACK;
        node->parent->parent->color = RED;
        insertFixup(mp,node->parent->parent);
        return;
    }

    /* Rotate red node to same hand line */
    if(node->parent->lchild == node && node->parent->parent->rchild == node->parent){
        rotateRight(mp,node->parent);
        node = node->rchild;
    }else if(node->parent->rchild == node && node->parent->parent->lchild == node->parent){
        rotateLeft(mp,node->parent);
        node = node->lchild;
    }

    node->parent->color = BLACK;
    node->parent->parent->color = RED;

    if(node->parent->lchild == node)
        rotateRight(mp,node->parent->parent);
    else
        rotateLeft(mp,node->parent->parent);
}

PRIVATE void removeFixup(struct map *mp, struct RBTNode *node)
{
    if(node->parent == NULL)   /* Every root to leaf path black node reduce one */
        return;

    struct RBTNode *brother = brotherNode(node);

    /* balance brother to balck node */
    if(brother->color == RED)
    {
        node->parent->color = RED;
        brother->color = BLACK;

        if(node->parent->lchild == node)
            rotateLeft(mp,node->parent);
        else
            rotateRight(mp,node->parent);
    }

    brother = brotherNode(node);

    /* Brother and tow brother child is not red */
    if(node->parent->color != RED &&
        brother->color != RED &&
        brother->lchild->color != RED &&
        brother->rchild->color != RED)
    {
        brother->color = RED;  /* Brother reduce one black note(less one note) lift */
        removeFixup(mp,node->parent);
    }
    else
    {
        if( node->parent->color == RED &&
            brother->color != RED &&
            brother->lchild->color != RED &&
            brother->rchild->color != RED)
        {
            brother->color = RED;
            node->parent->color = BLACK;  /* Increase one balance black node */
        }
        else
        {
            /* Near nephew red far nephew black */
            if(node->parent->lchild == node &&
                brother->lchild->color == RED &&
                brother->rchild->color != RED)
            {
                brother->color = RED;
                brother->lchild->color = BLACK;
                rotateRight(mp,brother);
            }
            else if(node->parent->rchild == node &&
                brother->lchild->color != RED &&
                brother->rchild->color == RED)
            {
                brother->color = RED;
                brother->rchild->color = BLACK;
                rotateLeft(mp,brother);
            }
            /* Far nephew red near nephew any color */
            brother = brotherNode(node);

            brother->color = node->parent->color; /* Root color recovery */
            node->parent->color = BLACK;

            if(node->parent->lchild == node)
            {
                brother->rchild->color = BLACK;
                rotateLeft(mp,node->parent);
            }
            else
            {
                brother->lchild->color = BLACK;
                rotateRight(mp,node->parent);
            }
        }
    }
}

void MapNew(struct map *mp, comparefunc cmp)
{
    mp->root = (struct RBTNode *)Mem_alloc(sizeof(struct RBTNode));
    mp->root->lchild = mp->root->rchild = mp->root->parent = NULL;
    mp->root->color  = NIL;
    mp->cmp = cmp;
}

/* Binary sort tree search */
struct Pair *MapFind(struct map *mp, void *key)
{
    struct RBTNode *tmpNode = mp->root;

    while(tmpNode->color != NIL)
    {
        if(!mp->cmp(tmpNode->obj.key, key))
        {
            return &tmpNode->obj;
        }

        if(mp->cmp(tmpNode->obj.key, key) > 0) /* greater search right */
        {
            tmpNode = tmpNode->lchild;
        }
        else
        {
            tmpNode = tmpNode->rchild;                      /* lower search left */
        }
    }

    return NULL;
}

/* from the left leaf remove node by bottom to top */
void MapClear(struct map *mp, pairfreefunc pairfree)
{
    struct RBTNode *node = mp->root, *tmp;

    while(node != NULL)
    {
        if(node->color == NIL || (node->lchild == NULL && node->rchild == NULL))
        {
            if(pairfree && node->color != NIL)(*pairfree)(&node->obj); /* pair free have color node */
            tmp = node;
            node = node->parent;
            if(node != NULL)
            {
                if(tmp == node->lchild)
                    node->lchild = NULL;
                else
                    node->rchild = NULL;
            }
            Mem_free(tmp);
        }

        if(node != NULL)
        {
            if(node->lchild != NULL)
                node = node->lchild;
            else if(node->rchild != NULL)
                node = node->rchild;
        }
    }
}

void MapInsert(struct map *mp, struct Pair pair)
{
    struct RBTNode *tmpNode = mp->root;

    while(tmpNode->color != NIL)
    {
        if(!mp->cmp(tmpNode->obj.key, pair.key))
        {
            tmpNode->obj = pair;
            return;
        }

        if(mp->cmp(tmpNode->obj.key, pair.key) > 0)
            tmpNode = tmpNode->lchild;
        else
            tmpNode = tmpNode->rchild;
    }

    tmpNode->color = RED;
    tmpNode->obj = pair;

    /* build two leaf node NIL*/
    tmpNode->lchild = (struct RBTNode *)Mem_alloc(sizeof(struct RBTNode));
    tmpNode->rchild = (struct RBTNode *)Mem_alloc(sizeof(struct RBTNode));

    tmpNode->lchild->parent = tmpNode;
    tmpNode->rchild->parent = tmpNode;

    tmpNode->lchild->color = NIL;
    tmpNode->rchild->color = NIL;

    insertFixup(mp,tmpNode);
}

void MapRemove(struct map *mp, void *key, pairfreefunc pairfree)
{
    struct RBTNode *node = mp->root;

    while(node->color != NIL)
    {
        if(!mp->cmp(node->obj.key,key)){
            if(pairfree)(*pairfree)(&node->obj); /* pair free find node */
            break;
        }

        if(mp->cmp(node->obj.key, key) > 0)
            node = node->lchild;
        else
            node = node->rchild;
    }

    if(node->color == NIL)
        return;

    if(node->rchild->color == NIL && node->lchild->color == NIL){
        /* No child */
        Mem_free(node->rchild);
        Mem_free(node->lchild);

        if(node->color == RED)
        {
            node->color = NIL;
        }else{
            node->color = NIL;
            removeFixup(mp,node);
        }
    }else{
        /* replace is black-black node
        ** tmpDel only have one child */

        struct RBTNode *tmpDel, *replace;

        if(node->lchild->color != NIL){
            tmpDel = node->lchild;        /* If left child exists */
            while(tmpDel->rchild->color != NIL)
                tmpDel = tmpDel->rchild;  /* Find precursor node repalcing delete node */
        }else{
            tmpDel = node->rchild;
            while(tmpDel->lchild->color != NIL)
                tmpDel = tmpDel->lchild;  /* Find successor node repalcing delete node */
        }

        node->obj = tmpDel->obj;

        replace = tmpDel->lchild->color != NIL ? tmpDel->lchild : tmpDel->rchild;

        if(replace == tmpDel->lchild)
            Mem_free(tmpDel->rchild);     /* Remove tmpDel NIL node */
        else
            Mem_free(tmpDel->lchild);

        replace->parent = tmpDel->parent;

        if(replace->parent == NULL)
            mp->root = replace;
        else if(tmpDel->parent->lchild == tmpDel)
            tmpDel->parent->lchild = replace;
        else
            tmpDel->parent->rchild = replace;

        /* if tmpDel is RED delete will not broken balance */
        if(tmpDel->color == BLACK){
            if(replace->color == RED)
                replace->color = BLACK;  /* Back fill a black node balance black node height */
            else
                removeFixup(mp,replace);
        }
        Mem_free(tmpDel);
    }

}

void MapInOrderHelp(struct RBTNode *r)
{
    if (r != 0 && r->color != NIL){
        MapInOrderHelp(r->lchild);	                                                        /* visit root left child */
        printf("key->(%s): \n  value->(%s)\n",(char*)r->obj.key,(char*)r->obj.value);	        /* visit root */
        MapInOrderHelp(r->rchild);	                                                        /* visit root right child */
    }
}
