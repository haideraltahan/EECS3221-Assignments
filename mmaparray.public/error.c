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
      fprintf(stderr, "%s: %s: %s\n",name, mssg, strerror(lerrno));
    else
      fprintf(stderr, "%s: %s\n",name, strerror(lerrno));
  else
    if (mssg != NULL)
      fprintf(stderr, "%s: %s\n",name, mssg);
    else
      fprintf(stderr, "%s: Unknown Error\n",name);
  exit(1);
}
