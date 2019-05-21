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
    int prg1_s, prg2_s = 0;
    int array_init_malloc = 10;
    int size_of_each_string = 50;
    char **prg1 = calloc(array_init_malloc, size_of_each_string * sizeof(char*));
    char **prg2;
    for (char **a = argv ; a != argv+argc ; a++) {
        // Skip `marker` and `-p-`
        if(strcmp(*a, "-p-") == 0){
            prg2 = calloc(array_init_malloc, size_of_each_string * sizeof(char*));
            i = 1;
            j = 0;
            continue;
        }
        if(strstr(*a, "marker") != NULL){
            continue;
        }
        if(j > array_init_malloc && i == 1){
            prg2 = realloc(prg2, size_of_each_string * (j+1) * sizeof(char *));
        } else {
            prg1 = realloc(prg1, size_of_each_string * (j+1) * sizeof(char *));
        }
        if(i == 1){
            prg2_s = j;
            prg2[j++] = *a;
        } else {
            prg1_s = j;
            prg1[j++] = *a;
        }
    }

    //printf("%s\n", prg1[1]);
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

    //read fdin and add it to argv
    char *input_file = calloc(1000, sizeof(char));
    unsigned char c;
    size_t n = 0;
    while (read(fdin, &c, 1) > 0)
    {
        input_file[n++] = (char) c;
    }
    input_file[n] = '\0';

    prg1[++prg1_s] = input_file;
    was_alarm = start_child(prg1[0], prg1, fdin, fdout, fderr1);
    signal(SIGALRM, alrm_handler);
    alarm(3);
    wait(&w_status);
    fprintf(stdout, "Process %s finished with status %d\n", prg1[0], w_status);

    if(was_alarm > 0){
        fprintf(stderr, "marker: At least one process did not finish\n");
    }
    // CLOSE ALL FILES AT THE END
    close(fdin);
    close(fdout);
    close(fderr1);
    close(fderr2);
}
