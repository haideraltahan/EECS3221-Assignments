enum STATES {
    THINKING, HUNGRY, EATING
};
struct thread_arg
{
    pthread_mutex_t *mutex;
    enum STATES state;
    unsigned int seed;		/* Needed for the random number genarator */
};
