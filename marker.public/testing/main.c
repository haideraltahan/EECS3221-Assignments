#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <signal.h>
#include <errno.h>

#include "fork.h"

int was_alarm = 0;

/* The main program does much of the work. parses the command line arguments */
/* sets up the alarm and the alarm signal handler opens the files and pipes */
/* for redirection etc., invoke start_child, closes files that should be  */
/* closed, waits for the children to finish and reports their status */
/* or exits printing a message and kill its children if they do not die */
/* in time (just the parent exiting may not be enough to kill the children) */
int main(int argc, char *argv[]) {
	//Get the input and format it
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
	//Format the input for exec later
	char *command1[p_position];
	char *command2[argc - p_position];
	int current = 1;
	while (current < p_position) {
		command1[current - 1] = argv[current];
		current++;
	}
	command1[current - 1] = (char *) NULL;

	current = 0;
	while (p_position + 1 + current < argc) {
		command2[current] = argv[p_position + 1 + current];
		current++;
	}
	command2[current] = (char *) NULL;

	//Declare pipes and files needed and set up alarm
	int fd[2];
	pid_t pid1, pid2, wid;
	remove("test.out"); remove("error.in"); remove("error.out");//remove previous files
	int in_file = open("test.in", O_RDWR | O_CREAT, 0666); //create the ones we need to read/write
	int out_file = open("test.out", O_RDWR | O_CREAT, 0666);
	int in_error = open("error.in", O_RDWR | O_CREAT, 0666);
	int out_error = open("error.out", O_RDWR | O_CREAT, 0666);
	signal(SIGALRM, alrm_handler);
	alarm(3);
	int status = -1; //return status for pid1 and pid2

	//Open the pipe
	if (pipe(fd) < 0) {
		f_error("pipe");
	}

	//Run each child process and close files
	pid1 = start_child(command1[0], command1, in_file, fd[1], in_error);
	close(fd[1]);
	pid2 = start_child(command2[0], command2, fd[0], out_file, out_error);
	close(fd[0]); close(in_file); close(out_file); close(in_error); close(out_error);

	//Check signals
	while(1) {
		wid = waitpid(pid1, &status, 0);
		if (was_alarm) {
			fprintf(stderr, "At least one process didn't finish\n");
			kill(pid1, SIGKILL);
			exit(0);
		}
		else {
			break;
		}
	}

  	if (wid == pid1)
    	printf("Process %s finished with status %d\n", command1[0], status);
  	if (wid == pid2)
    	printf("Process %s finished with status %d\n", command2[0], status);

  	while(1) {
		wid = waitpid(pid2, &status, 0);
		if (was_alarm) {
			fprintf(stderr, "At least one process didn't finish\n");
			kill(pid2, SIGKILL);
			exit(0);
		}
		else {
			break;
		}
	}

  	if (wid == pid1)
    	printf("Process %s finished with status %d\n", command1[0], status);
  	if (wid == pid2)
    	printf("Process %s finished with status %d\n", command2[0], status);
	
}