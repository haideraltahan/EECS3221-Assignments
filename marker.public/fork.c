#include "fork.h"

/* This is the handler of the alarm signal. It just updates was_alarm */
void alrm_handler(int i)
{
}

/* Prints string s using perror and exits. Also checks errno to make */
/* sure that it is not zero (if it is just prints s followed by newline */
/* using fprintf to the standard error */
void f_error(char *s)
{
	//errorno is a variable in the errno.h header file. It is set by itself when using system calls.
	//It gets set but should never be 0, as this means no error
	if (errno == 0)
		fprintf(stderr, "%s\n", s);
	else { //errno != 0
		perror(s); //This prints "whatever string s is : the desription of the error" automatically
		//e.g. Error: : No such file or directory if we pass s as "Error" and the last system call tried to open a file
		//and that file didn't exist.
	    // Needs to exit the program. According to the documentation above.
	    exit(1);
	}
}

/* Creates a child process using fork and a function from the exec family */
/* The standard input output and error are replaced by the last three */
/* arguments to implement redirection and piping */
pid_t start_child(const char *path, char *const argv[],
		  int fdin, int fdout, int fderr)
{
    pid_t child = fork();
    if(child == -1){
        f_error("Failed to fork child");
    }
    if(child == 0){
        /* Close stdin, duplicate the input side of pipe to fdout */
        close(fdin); // Should make it read...
        dup2(fdout, 1); // redirect stdout
        dup2(fderr, 2); // to write to test.err1/2
        execvp(path, argv);
        close(fdout);
        close(fderr);
        f_error("Failed to exec child");
    }
    return child;
}
