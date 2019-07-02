//  Author: Minas Spetsakis
//  Descrn: A short demo program about threads similar to the one in the book
//  Date:  Sep. 28 2016

#include <pthread.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>


#include "runner.h"

int main(int argc, char *argv[])
{
  pthread_t tid1, tid2;
  pthread_attr_t attr;
  thread_info info1, info2;
  
  info1.cnt=0;
  info1.who=1;
  info1.other = &info2;

  info2.cnt=0;
  info2.who=2;
  info2.other = &info1;

  printf("I am mama thread and my PID is %d\n",getpid());

  pthread_attr_init(&attr);

  pthread_create(&tid1, &attr, runner, (void*)&info1);
  pthread_create(&tid2, &attr, runner, (void*)&info2);

  pthread_join(tid1,NULL);
  pthread_join(tid2,NULL);
}

