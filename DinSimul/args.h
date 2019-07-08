struct thread_arg
{
  int chops[2];
  pthread_mutex_t *clk_mutex, *philosopher_mutex;
  unsigned int seed;		/* Needed for the random number genarator */
};
