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
      // This means that is it not of the form `marker ls -p- wc
      if(argc < 4){
          f_error("There are no enough arguments");
      }

      // Another check for the case where `-p-` does not exist in argv
      int i = 0;
       for (char **a = argv ; a != argv+argc ; a++) {
        if(strcmp(*a, "-p-")){
            i = 1;
            break;
        }
        }
        if(!i){
            f_error("Failed, -p- is missing");
        }

    // Loop over argv to get the first and second commands.
    i = 0;
    char *prg1[2];
    char *prg2[2];
    for (char **a = argv ; a != argv+argc ; a++) {
        // Skip `marker` and `-p-`
        if(!strcmp(*a, "-p-") || strstr(*a, "marker") != NULL){
            continue;
        }

        //TODO: ADD the programs to `prg1` and `prg2` with their arguments
    }

    //Make first child --test--
    int fdin = open("test.in", O_RDWR, 0777);
    int fdout = open("test.out", O_RDWR, 0777);
    int fderr = open("test.err1", O_RDWR, 0777);
    int p[2];
    p[0] = fdin;
    p[1] = fdout;
    if (pipe(p) == -1){
        f_error("Failed to create pipe in child");
    }
    start_child(NULL, NULL, fdin, fdout, fderr);

}
