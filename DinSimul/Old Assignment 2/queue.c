#include <stdio.h>
#include <malloc.h>
#include <stdlib.h>
#include <pthread.h>

#include "queue.h"
#include "error.h"

queue_t *mk_queue()
{
  queue_t *q;
  int pthrerr;

  q = (queue_t *)malloc(sizeof(queue_t));
  if (q == NULL)
    fatalerr("Queue", 0, "Out of memory\n");
  q->n = 0;
  pthrerr = pthread_mutex_init(&(q->qmutex), NULL);
  if (pthrerr!=0)
    fatalerr("Queue",pthrerr,"Mutext initialization failed\n");
  return q;
}

void free_queue(queue_t *q)
{
  int pthrerr;

  pthrerr = pthread_mutex_destroy(&(q->qmutex));
  if (pthrerr!=0)
    fatalerr("Queue",pthrerr,"Mutex destroy failed\n");
  free(q);
}

void push_q(queue_t *q)
{
  int pthrerr;

  pthrerr = pthread_mutex_lock(&(q->qmutex));
  if (pthrerr!=0)
    fatalerr("Queue",pthrerr,"Mutex lock failed\n");
  (q->n)++;
  pthrerr = pthread_mutex_unlock(&(q->qmutex));
  if (pthrerr!=0)
    fatalerr("Queue",pthrerr,"Mutex unlock failed\n");
}

void pop_q(queue_t *q)
{
  int pthrerr;

  pthrerr = pthread_mutex_lock(&(q->qmutex));
  if (pthrerr!=0)
    fatalerr("Queue",pthrerr,"Mutex lock failed\n");
  if (q->n<=0)
    fatalerr("Queue", 0, "Attempt to pop from an empty queue\n");
  (q->n)--;
  pthrerr = pthread_mutex_unlock(&(q->qmutex));
  if (pthrerr!=0)
    fatalerr("Queue",pthrerr,"Mutex unlock failed\n");
}

int safepop_q(queue_t *q)
{
  int pthrerr;

  pthrerr = pthread_mutex_lock(&(q->qmutex));
  if (pthrerr!=0)
    fatalerr("Queue",pthrerr,"Mutex lock failed\n");
  if (q->n<=0)
    {
      pthrerr = pthread_mutex_unlock(&(q->qmutex));
      if (pthrerr!=0)
	fatalerr("Queue",pthrerr,"Mutex unlock failed\n");
      return 0;
    }
  (q->n)--;
  pthrerr = pthread_mutex_unlock(&(q->qmutex));
  if (pthrerr!=0)
    fatalerr("Queue",pthrerr,"Mutex unlock failed\n");
  return 1;
}

int size_q(queue_t *q)
{
  return q->n;
}

