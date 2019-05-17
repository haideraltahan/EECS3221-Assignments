#include "fork.h"

/* This is the handler of the alarm signal. It just updates was_alarm */
void alrm_handler(int i)
{
}

/* Prints string s using perror and exits. Also checks errno to make */
/* sure that it is not zero (if it is just prints s followed by newline */
/* using fprintf to the standard error */
void f_error(char *s, int i)
{
    perror(s);
    FILE * fp;

    if(i == 1){
        fp = fopen ("test.err1", "w");
    }else{
        fp = fopen ("test.err2", "w");
    }
    if(errno == 0){
        fprintf(fp, "%s\n", s);
    }

    fclose(fp);
    exit(0);
}

/* Creates a child process using fork and a function from the exec family */
/* The standard input output and error are replaced by the last three */
/* arguments to implement redirection and piping */
pid_t start_child(const char *path, char *const argv[],
		  int fdin, int fdout, int fderr)
{

}
