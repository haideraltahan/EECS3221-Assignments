struct thread_arg
{
  int chops[2];
  pthread_cond_t *chopsticks_cond, *chairs_cond;
  int *chopsticks, *chairs;
  pthread_mutex_t *clk_mutex, *philosopher_mutex;
  unsigned int seed;		/* Needed for the random number genarator */
};
