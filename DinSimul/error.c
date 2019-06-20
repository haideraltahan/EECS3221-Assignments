#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/************************************************************************/
/**********                      fatalerr                      **********/
/************************************************************************/

void fatalerr(char *name, int lerrno, char *mssg)
{
  if (lerrno>0)
    if (mssg != NULL)
      fprintf(stderr, "%s(%d): %s: %s\n",name, getpid(), mssg, strerror(lerrno));
    else
      fprintf(stderr, "%s(%d): %s\n",name, getpid(), strerror(lerrno));
  else
    if (mssg != NULL)
      fprintf(stderr, "%s(%d): %s\n",name, getpid(), mssg);
    else
      fprintf(stderr, "%s(%d): Unknown Error\n",name, getpid());
  exit(1);
}

/***********************************************************************
                           r a n d 0 _ 1
************************************************************************/
double rand0_1(unsigned int *seedp)
{
  double f;
  /* We use the re-entrant version of rand */
  f = (double)rand_r(seedp);
  return f/(double)RAND_MAX;
}

