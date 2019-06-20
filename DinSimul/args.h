struct thread_arg
{
  int N, T;
  float lam, mu;
  pthread_cond_t *whatever;	/* find a better name */
  pthread_mutex_t *blah;
  unsigned int seed;		/* Needed for the random number genarator */
};
