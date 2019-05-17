#include "fork.h"

int was_alarm=0;

/* The main program does much of the work. parses the command line arguments */
/* sets up the alarm and the alarm signal handler opens the files and pipes */
/* for redirection etc., invoke start_child, closes files that should be  */
/* closed, waits for the children to finish and reports their status */
/* or exits printing a message and kill its children if they do not die */
/* in time (just the parent exiting may not be enough to kill the children) */
int main(int argc, char *argv[])
{
  if(argc < 4){
      f_error("There are no enough arguments");
  }

    char *a[2];
    int issecond = 0;
    char **arg;
    for (arg = argv; *arg; ++arg) { // for each arg
        if(*arg == "-p-"){
            issecond = 1;
            continue;
        }
        printf("%s",*arg);
        char *p;
        for (p = *arg; *p; ++p) {      // for each character
            if(issecond){
                strcat(a[1],p);
            }else{
                strcat(a[0],p);
            }
        }
    }

}
