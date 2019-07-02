//  Author: Minas Spetsakis
//  Descrn: A short demo program about threads similar to the one in the book
//  Date  :  Sep. 28 2016

#define _GNU_SOURCE
#include <unistd.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <stdio.h>
#include <pthread.h>

#include "runner.h"

void *runner(void *vinfo)
{
  int i;
  pid_t my_pid, my_tid;
  thread_info *info;

  info = (thread_info *)vinfo;

  my_pid = syscall(SYS_getpid);
  my_tid = syscall(SYS_gettid);

  for (i=0; i<10; i++)
    {
      printf("%d: I am %d, with PID %d and TID %d\n",
	     info->cnt++, info->who, my_pid, my_tid);
    }
  pthread_exit(0);
}

