struct thread_arg
{
    pthread_cond_t *start_line_cond, *clk_cond;
    pthread_mutex_t *mutex;
    unsigned int seed;		/* Needed for the random number genarator */
};
