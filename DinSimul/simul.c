/***********************************************************************/
/**      Author: Minas Spetsakis                                      **/
/**        Date: Nov. 10 2016                                         **/
/** Description: Skeleton code for Assgn. II                               **/
/***********************************************************************/

#include <stdio.h>
#include <sys/time.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>

#include "args.h"
#include "error.h"

int N=5, T=100;
double lam=0.1, mu=0.2;
int nblocked;			/* The number of threads blocked */


/***********************************************************************
                             C L I E N T
************************************************************************/

void *client(void *vptr)
{
  unsigned int seed;
  int pthrerr;
  struct thread_arg *ptr;

  ptr = (struct thread_arg*)vptr;

  while (1)
    {
      fatalerr("client", 0, "Not yet implemented");
    }
  return NULL;
      
}

/***********************************************************************
                             S E R V E R
************************************************************************/

void *server(void *vptr)
{
  int busy;
  int pthrerr;
  struct thread_arg *ptr;

  ptr = (struct thread_arg*)vptr;

  while (1)
    {
      fatalerr("server", 0, "Not yet implemented");
    }
  return NULL;
}


/***********************************************************************
                                C L K
************************************************************************/

void *clk(void *vptr)
{
  int tick;
  int pthrerr;
  struct thread_arg *ptr;

  ptr = (struct thread_arg*)vptr;

  fatalerr("server", 0, "Not yet implemented");
  exit(0);
}

int main(int argc, char **argv)
{
  int pthrerr, i;
  int nserver, nclient, nticks;
  float lam, mu;

  pthread_t whatever;
  pthread_cond_t something;
  pthread_mutex_t blah;
  struct thread_arg *allargs;
  pthread_t *alltids;


  printf("Should not be here\n");
  exit(-1);
}
