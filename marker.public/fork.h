#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

extern int was_alarm;

extern void alrm_handler(int i);
extern void f_error(char *s, int i);
extern pid_t start_child(const char *path, char *const argv[],
			 int fdin, int fdout, int fderr);

