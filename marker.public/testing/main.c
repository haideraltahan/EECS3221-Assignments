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
	char p_position = 0; //start looking at the third argument (after ./marker)
	for (int i = 0; i < argc; i++) {
		if (strcmp(argv[i], "-p-") == 0) {
			p_position = i;
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

	printf("p position: %d\n", p_position);
	char *command1[p_position];
	char *command2[argc - p_position];
	int current = 1;
	while (current < p_position) {
		command1[current - 1] = argv[current];
		printf("%s\n", command1[current - 1]);
		printf("%d\n", current);
		current++;
	}
	command1[current - 1] = (char *) NULL;

	current = 0;
	while (p_position + 1 + current < argc) {
		command2[current] = argv[p_position + 1 + current];
		printf("%s\n", command2[current]);
		printf("%d\n", current);
		current++;
	}
	command2[current] = (char *) NULL;
	

	int fd[2];
	pid_t pid1, pid2, wid;
	remove("test.out"); remove("error.in"); remove("error.out");//remove previous files
	int in_file = open("test.in", O_RDWR | O_CREAT, 0666); //create the ones we need to read/write
	int out_file = open("test.out", O_RDWR | O_CREAT, 0666);
	int in_error = open("error.in", O_RDWR | O_CREAT, 0666);
	int out_error = open("error.out", O_RDWR | O_CREAT, 0666);
	char buffer[1000] = {};
	printf("%d %d %d %d\n", in_file, out_file, in_error, out_error);

	if (pipe(fd) < 0) {
		printf("Pipe creation error\n");
		exit(-1);
	}

	pid1 = fork();
	if (pid1 < 0) {
		printf("Fork pid1 failed.\n");
		exit(-1);
	}
	if (pid1 == 0) {
		printf("pid1 forked.\n");
		close(fd[0]);
		if (dup2(in_file, 0) < 0 || dup2(fd[1], 1) < 0) {
			printf("pid1 failed dup2.\n");
			exit(-1);
		}
		close(fd[1]); close(in_file);
		char *cmd[] = {"wc", NULL};
		execvp(command1[0], command1);
	}
	wait();
	pid2 = fork();
	if (pid2 < 0) {
		printf("Fork pid2 failed.\n");
		exit(-1);
	}
	if (pid2 == 0) {
		printf("pid2 forked.\n");
		close(fd[1]);
		//read(fd[0], buffer, sizeof(buffer));
		//printf("Here is the buffer's contents: %s", buffer);
		if (dup2(fd[0], 0) < 0 || dup2(out_file, 1) < 0) {
			printf("pid2 failed dup2\n");
		}
		close(fd[0]); close(out_file);
		char *cmd[] = {"cat", NULL};
		execvp(command2[0], command2);

	}
	close(fd[0]); close(fd[1]);
	wait();
	/*
	wid = wait(NULL);
  	if (wid == pid1)
    	printf("Process %d (%s) finished\n",wid, argv[1]);
  	if (wid == pid2)
    	printf("Process %d (%s) finished\n",wid, argv[p_position]);
  	wid = wait(NULL);
  	if (wid == pid1)
    	printf("Process %d (%s) finished\n",wid, argv[1]);
  	if (wid == pid2)
    	printf("Process %d (%s) finished\n",wid, argv[p_position]);
    */

	
	
}