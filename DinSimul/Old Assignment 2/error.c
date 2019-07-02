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
