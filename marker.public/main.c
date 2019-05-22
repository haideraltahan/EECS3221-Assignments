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
    char p_position = 0; //start looking at the third argument (after ./marker)
    int i;
	for (i = 0; i < argc; i++) {
		if (strcmp(argv[i], "-p-") == 0) {
			p_position = i+1;
			argv[i] = NULL;
			break;
		}
	}
	if (!p_position) {
		f_error("-p- was not found as an argument.");
	}

	struct sigaction sa;
	sa.sa_handler = alrm_handler;
	if (sigaction(SIGALRM, &sa, NULL) < 0) f_error("sigaction failed.");

    //printf("%s\n", prg1[1]);
    //Make first child --test--
    int fdin = open("test.in", O_RDONLY, 0777);
    int fdout = open("test.out", O_RDWR | O_TRUNC, 0777);
    int fderr1 = open("test.err1", O_RDWR | O_TRUNC, 0777);
    int fderr2 = open("test.err2", O_RDWR | O_TRUNC, 0777);
    if (fdin < 0 || fdout < 0 || fderr1 < 0 || fderr2 < 0) {
		f_error("One or more files needed failed to open.\n");
	}

    int p[2];
    if (pipe(p) == -1){
        f_error("Failed to create pipe");
    }

    pid_t children[2];
    // run the first child
    children[0] = start_child(argv[1], &argv[1], fdin, p[1], fderr1);
    if (close(p[1]) < 0) f_error("Failed to close a pipe.");
    // run second child
    children[1] = start_child(argv[p_position], &argv[p_position], p[0], fdout, fderr2);
    alarm(3);
    if (close(p[0]) < 0) f_error("Failed to close a pipe.");

    // CLOSE ALL FILES AT THE END
    close(fdin);
    close(fdout);
    close(fderr1);
    close(fderr2);

    int w_status = 0;
    pid_t child;
    for(i =0; i <2;i++){
        child = wait(&w_status);
        if(was_alarm){
            if (kill(children[i], SIGKILL) < 0) {f_error("Failed to close the first process.\n");}
        }
        if(i == 1){
            fprintf(stdout, "Process %s finished with status %d\n", argv[p_position], w_status);
        }
        if(i == 0){
            fprintf(stdout, "Process %s finished with status %d\n", argv[1], w_status);
        }
    }

    if(was_alarm == 1){
        fprintf(stderr, "marker: At least one process did not finish\n");
    }

    return 0;
}
