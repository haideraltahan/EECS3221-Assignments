struct thread_arg
{
  int nserver, nclient, nticks;
  queue_t *q;
  float lam, mu;
  pthread_cond_t *thrblockcond, *clkblockcond;
  pthread_mutex_t *blocktex, *statex;
  unsigned int seed;
};

