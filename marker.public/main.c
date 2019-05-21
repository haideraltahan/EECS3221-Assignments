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
    int j = 0;
    int w_status;
    char **prg1 = malloc(sizeof(char*));
    char **prg2 = malloc(sizeof(char*));
    for (char **a = argv ; a != argv+argc ; a++) {
        // Skip `marker` and `-p-`
        if(strcmp(*a, "-p-") == 0){
            i = 1;
            j = 0;
            continue;
        }
        if(strstr(*a, "marker") != NULL){
            continue;
        }
        if(i == 1){
            prg2 = (char **)realloc(prg2, (j+1) * sizeof(char));
            prg2[j] = *a;
        } else {
            prg1 = (char **)realloc(prg1, (j+1) * sizeof(char));
            prg1[j] = *a;
        }
        j++;
    }

    //Make first child --test--
    int fdin = open("test.in", O_RDONLY, 0777);
    int fdout = open("test.out", O_RDWR | O_TRUNC, 0777);
    int fderr1 = open("test.err1", O_RDWR | O_TRUNC, 0777);
    int fderr2 = open("test.err2", O_RDWR | O_TRUNC, 0777);
//    int p[2];
//    p[0] = fdin;
//    p[1] = fdout;
//    if (pipe(p) == -1){
//        f_error("Failed to create pipe in child");
//    }

    was_alarm = start_child(prg1[0], prg1, fdin, fdout, fderr1);
    signal(SIGALRM, alrm_handler);
    alarm(3);
    wait(&w_status);
    fprintf(stdout, "Process %s finished with status %d\n", prg1[0], w_status);

    fprintf(stdout, "%d\n",was_alarm);
    if(was_alarm > 0){
        fprintf(stderr, "marker: At least one process did not finish\n");
    }

    // CLOSE ALL FILES AT THE END
    close(fdin);
    close(fdout);
    close(fderr1);
    close(fderr2);
}
