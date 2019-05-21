#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>

#include "fork.h"

int was_alarm=0;

/* The main program does much of the work. parses the command line arguments */
/* sets up the alarm and the alarm signal handler opens the files and pipes */
/* for redirection etc., invoke start_child, closes files that should be  */
/* closed, waits for the children to finish and reports their status */
/* or exits printing a message and kill its children if they do not die */
/* in time (just the parent exiting may not be enough to kill the children) */
int main(int argc, char *argv[]) {

	if (argc < 4) {
		printf("Too few arguments given. There must be at least 4.\n");
		exit(-1);
	}
	char *p_position = NULL; //start looking at the third argument (after ./marker)
	for (int i = 0; i < argc; i++) {
		if (strcmp(argv[i], "-p-") == 0) {
			p_position = argv[i];
			argv[i] = (char *) NULL;
			break;
		}

	}
	if (!p_position) {
		printf("-p- was not found as an argument.\n");
		exit(-1);
	}
	if (p_position == argv[1] || p_position == argv[argc - 1]) {
		printf("-p- cannot be the first or last argument. It must connect two other commands.\n");
		exit(-1);
	}

	printf("%d\n", p_position);
	int fd[2];
	pid_t pid;
	remove("test.out"); //remove previous files
	int in_file = open("test.in", O_RDWR | O_CREAT, 0666); //create the ones we need to read/write
	int out_file = open("test.out", O_RDWR | O_CREAT, 0666);
	printf("%d %d\n", in_file, out_file);

	if (pipe(fd) < 0) {
		printf("Pipe creation error\n");
		exit(-1);
	}

	if ((pid = fork()) < 0) {
		printf("Fork error.\n");
		exit(-1);
	}
	if (pid == 0) { //Child
		printf("Child\n");
		close(fd[0]);
		close(0); //close stdin
		dup(in_file); //replace it with in_file
		close(1); //close stdout
		dup(out_file); //replace with out_file
		close(2); //close stderror for now
		close(in_file);
		close(out_file);
		char *cmd[] = {"wc", NULL};
		execvp(cmd[0], cmd);

	}
	if (pid > 0) {//Parent
		printf("Parent\n");
		wait();
		close(fd[1]);
		close(3); close(4);
	}
	exit(0);
	
	
}