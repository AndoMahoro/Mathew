#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <assert.h>
#include "mathew.h"

/* Template ring queue */
PRIVATE void move(struct rqueue *rqp, unsigned long *p)
{
    (*p)++;
    (*p) %= rqp->qSize;
}

PRIVATE int isFull(struct rqueue *rqp)
{
    return rqp->qLength == rqp->qSize;
}

PRIVATE int isEmpty(struct rqueue *rqp)
{
    return rqp->qLength == 0;
}

unsigned long RingQueueLength(struct rqueue *rqp)
{
    return rqp->qLength;
}

void PushRingQueue(struct rqueue *rqp, void *item)
{
   unsigned long i;
   unsigned long tail;
   if(isFull(rqp)){
       /* Auto growth */
       struct rqueue new_rq;
       new_rq.iLen = rqp->iLen;
       new_rq.iHead = new_rq.iTail = new_rq.qLength = 0;
       new_rq.qSize = rqp->qSize * 2 ;
       new_rq.pQueue = Mem_alloc(new_rq.iLen * new_rq.qSize);

       while(!isEmpty(rqp)){
           PopRingQueue(rqp,&(((char*)new_rq.pQueue)[new_rq.iTail * new_rq.iLen]));
           move(&new_rq,&new_rq.iTail);
           new_rq.qLength++;
       }
       *rqp = new_rq;  /* Updata queue */
   }

   tail = rqp->iTail * rqp->iLen;
   for(i = 0; i< rqp->iLen; i++){
       ((char*)rqp->pQueue)[tail + i] = ((char *)item)[i];
   }
   move(rqp,&rqp->iTail);
   rqp->qLength++;
   
}

int PopRingQueue(struct rqueue *rqp, void *item)
{
   if(isEmpty(rqp))return FALSE;
   unsigned long i;
   unsigned long head = rqp->iHead * rqp->iLen;

   for(i = 0; i< rqp->iLen; i++){
       ((char*)item)[i] = ((char*)rqp->pQueue)[head + i];
   }
   move(rqp,&rqp->iHead);
   rqp->qLength--;

   return TRUE;
}

/* Auto grow queue */
void InitRingQueue(struct rqueue *rqp, unsigned long ilen, unsigned long size)
{
   rqp->qSize = size;
   rqp->iLen = ilen;
   rqp->iHead = rqp->iTail = rqp->qLength = 0;
   rqp->pQueue = Mem_alloc(rqp->iLen * rqp->qSize);
   MEM_CHECK(rqp->pQueue);
}

void ResetRingQueue(struct rqueue *rqp)
{
    rqp->iHead = rqp->iTail = rqp->qLength = 0;
}

void DeleteRingQueue(struct rqueue *rqp)
{
    rqp->iLen = rqp->iHead = rqp->iTail = rqp->qLength = rqp->qSize = 0;
    Mem_free(rqp->pQueue);
}


