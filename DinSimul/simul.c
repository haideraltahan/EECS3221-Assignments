/***********************************************************************/
/**      Author: Minas Spetsakis                                      **/
/**        Date: Jun. 2019                                         **/
/** Description: Skeleton code for Assgn. II                               **/
/***********************************************************************/

#include <stdio.h>
#include <sys/time.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>

#include "args.h"
#include "error.h"

int N = 5, T = 100;
double lam = 0.1, mu = 0.2;
int nblocked, nWaiting_chair, nWaiting_chop, *nblocked_chop;            /* The number of threads blocked */
int nThink, nHungry, nEating = 0;
int *chopsticks, *chairs;
pthread_cond_t start_line_cond, clk_cond, phil_cond, chairs_cond, *chopsticks_cond;

/***********************************************************************
                         P H I L O S O P H E R
  Each thread run this piece of code. The while loop ensures that the
  threads loops until the clock stops and kills the treads.
************************************************************************/
void *philosopher(void *vptr) {
    int pthrerr;            /* creating a new thread               */
    struct thread_arg *ptr;

    ptr = (struct thread_arg *) vptr;

    /* waiting for all other threads to block on clk */
    pthrerr = pthread_mutex_lock(ptr->mutex);
    if (pthrerr != 0)
        fatalerr("Philosopher", pthrerr, "Mutex lock failed\n");
    nblocked++;
    if (nblocked == N) {            /* cond_signal never returns an error code */
        pthrerr = pthread_cond_signal(&clk_cond);
        if (pthrerr != 0)
            fatalerr("Philosopher", pthrerr, "Condition signal failed\n");
    }
    pthrerr = pthread_cond_wait(&start_line_cond, ptr->mutex);
    if (pthrerr != 0)
        fatalerr("Philosopher", pthrerr, "Condition wait failed\n");
    pthrerr = pthread_mutex_unlock(ptr->mutex);
    if (pthrerr != 0)
        fatalerr("Philosopher", pthrerr, "Mutex unlock failed\n");

    while (1) {
        pthrerr = pthread_mutex_lock(ptr->mutex);
        if (pthrerr != 0)
            fatalerr("Philosopher", pthrerr, "Mutex lock failed\n");

        /* This if statement checks if the philosopher is in the EATING or THINKING state.
         * If the philosopher is in the HUNGRY state then he/she would be stack
         *                  on a condition variable for either a chopstick or chair.
         * */
        if (ptr->state == EATING) {
            /* If random variable is less than mu then the philosopher goes from EATING state to THINKING state
             *              after releasing both chopsticks and chair. */
            if (rand0_1(&(ptr->seed)) < mu) {

                // Release of both chopsticks and chair
                *(chopsticks + (ptr->id % N)) = 1;
                *(chopsticks + ((ptr->id + 1) % N)) = 1;
                ++*chairs;

                /* Check if another philosopher is waiting on the left chopstick he released
                 * if he is then signal to it to wake up.
                 * */
                if (*(nblocked_chop + (ptr->id % N)) > 0) {
                    nWaiting_chop--;
                    *(nblocked_chop + (ptr->id % N)) = 0;
                    pthrerr = pthread_cond_signal((chopsticks_cond + (ptr->id % N)));
                    if (pthrerr != 0)
                        fatalerr("Philosopher", pthrerr, "Condition b/cast failed\n");
                }

                /* Check if another philosopher is waiting on the right chopstick he released
                 * if he is then signal to it to wake up.
                 * */
                if (*(nblocked_chop + ((ptr->id + 1) % N)) > 0) {
                    nWaiting_chop--;
                    *(nblocked_chop + ((ptr->id + 1) % N)) = 0;
                    pthrerr = pthread_cond_signal((chopsticks_cond + ((ptr->id + 1) % N)));
                    if (pthrerr != 0)
                        fatalerr("Philosopher", pthrerr, "Condition b/cast failed\n");
                }

                /* Check if another philosopher is waiting on the chair that he released
                 * if he is then signal to it to wake up.
                 * */
                if (nWaiting_chair == 1) {
                    nWaiting_chair = 0;
                    pthrerr = pthread_cond_broadcast(&chairs_cond);
                    if (pthrerr != 0)
                        fatalerr("Philosopher", pthrerr, "Condition b/cast failed\n");

                }

                // Switch to THINKING state
                ptr->state = THINKING;
            }
        } else {
            /*
             * If less than lamda then this philosopher switch to HUNGRY state.
             * Needs to go EATING when the chopsticks become available.
             * */
            if (rand0_1(&(ptr->seed)) < lam) {
                // Switch to HUNGRY state
                ptr->state = HUNGRY;

                // Wait for a chair if none is available.
                if (*chairs <= 0) {
                    nWaiting_chair = 1;
                    if (nblocked + nWaiting_chair + nWaiting_chop ==
                        N) {            /* cond_signal never returns an error code */
                        pthrerr = pthread_cond_signal(&clk_cond);
                        if (pthrerr != 0)
                            fatalerr("Philosopher", pthrerr, "Condition signal failed\n");
                    }
                    pthrerr = pthread_cond_wait(&chairs_cond, ptr->mutex);
                    if (pthrerr != 0)
                        fatalerr("Philosopher", pthrerr, "Conditionf wait failed\n");
                }
                // grab the chair
                --*chairs;

                // Wait for the left chopstick if not available.
                if (!*(chopsticks + (ptr->id % N))) {
                    nWaiting_chop++;
                    *(nblocked_chop + (ptr->id % N)) = 1;
                    if (nblocked + nWaiting_chair + nWaiting_chop ==
                        N) {            /* cond_signal never returns an error code */
                        pthrerr = pthread_cond_signal(&clk_cond);
                        if (pthrerr != 0)
                            fatalerr("Philosopher", pthrerr, "Condition signal failed\n");
                    }
                    pthrerr = pthread_cond_wait((chopsticks_cond + (ptr->id % N)), ptr->mutex);
                    if (pthrerr != 0)
                        fatalerr("Philosopher", pthrerr, "Conditionf wait failed\n");
                }

                // Takes the left chopstick
                *(chopsticks + (ptr->id % N)) = 0;

                // Wait for the right chopstick if not available.
                if (!*(chopsticks + ((ptr->id + 1) % N))) {
                    nWaiting_chop++;
                    *(nblocked_chop + ((ptr->id + 1) % N)) = 1;
                    if (nblocked + nWaiting_chair + nWaiting_chop ==
                        N) {            /* cond_signal never returns an error code */
                        pthrerr = pthread_cond_signal(&clk_cond);
                        if (pthrerr != 0)
                            fatalerr("Philosopher", pthrerr, "Condition signal failed\n");
                    }
                    pthrerr = pthread_cond_wait((chopsticks_cond + ((ptr->id + 1) % N)), ptr->mutex);
                    if (pthrerr != 0)
                        fatalerr("Philosopher", pthrerr, "Condition wait failed\n");
                }
                // Takes the right chopstick
                *(chopsticks + ((ptr->id + 1) % N)) = 0;

                // Switch to EATING
                ptr->state = EATING;
            }
        }

        ++nblocked;
        if (nblocked + nWaiting_chair + nWaiting_chop == N) {            /* cond_signal never returns an error code */
            pthrerr = pthread_cond_signal(&clk_cond);
            if (pthrerr != 0)
                fatalerr("Philosopher", pthrerr, "Condition signal failed\n");
        }
        pthrerr = pthread_cond_wait(&phil_cond, ptr->mutex);
        if (pthrerr != 0)
            fatalerr("Philosopher", pthrerr, "Condition wait failed\n");
        pthrerr = pthread_mutex_unlock(ptr->mutex);
        if (pthrerr != 0)
            fatalerr("Philosopher", pthrerr, "Mutex unlock failed\n");
    }
}

/***********************************************************************
                                C L K
   Computes statistics every tick. Then report the statistics at the end

************************************************************************/
void *clk(void *vptr) {
    int tick, i;
    int pthrerr;
    struct thread_arg *ptr;

    ptr = (struct thread_arg *) vptr;

    /* Waits for all thread to block */
    pthrerr = pthread_mutex_lock(ptr->mutex);
    if (pthrerr != 0)
        fatalerr("Clock", pthrerr, "Mutex lock failed\n");
    while (nblocked < N) {
        pthrerr = pthread_cond_wait(&clk_cond, ptr->mutex);
        if (pthrerr != 0)
            fatalerr("Clock", 0, "Condition wait failed\n");
    }
    nblocked = 0;
    /* Wakes all the threads up */
    pthrerr = pthread_cond_broadcast(&start_line_cond);
    if (pthrerr != 0)
        fatalerr("Clock", pthrerr, "Mutex unlock failed\n");
    pthrerr = pthread_mutex_unlock(ptr->mutex);
    if (pthrerr != 0)
        fatalerr("Clock", pthrerr, "Mutex unlock failed\n");

    /* starts the clock ticking */
    for (tick = 0; tick < T; tick++) {
        pthrerr = pthread_mutex_lock(ptr->mutex);
        if (pthrerr != 0)
            fatalerr("Clock", pthrerr, "Mutex lock failed\n");

        while (nblocked + nWaiting_chair + nWaiting_chop < N) {
            pthrerr = pthread_cond_wait(&clk_cond, ptr->mutex);
            if (pthrerr != 0)
                fatalerr("Clock", 0, "Condition wait failed\n");
        }

        /* computes the statistics */
        for (i = 0; i < N; i++) {
            if ((ptr + i)->state == EATING) {
                nEating++;
            } else if ((ptr + i)->state == HUNGRY) {
                nHungry++;
            } else {
                nThink++;
            }
        }

        /* broadcast to wake all threads */
        nblocked = 0;
        pthrerr = pthread_cond_broadcast(&phil_cond);
        if (pthrerr != 0)
            fatalerr("Clock", pthrerr, "Mutex unlock failed\n");
        pthrerr = pthread_mutex_unlock(ptr->mutex);
        if (pthrerr != 0)
            fatalerr("Clock", pthrerr, "Mutex unlock failed\n");
    }

    /* prints statistics */
    printf("Average Thinking Time: %6.2f\n", (float) nThink / (float) (T * N));
    printf("Average Hungry Time: %6.2f\n", (float) nHungry / (float) (T * N));
    printf("Average Eating Time: %6.2f\n", (float) nEating / (float) (T * N));
    exit(0);
}

/***********************************************************************
                               M A I N
   Responsible for initializing the global variables and all threads.
   Moreover, initializes the clock.
************************************************************************/
int main(int argc, char **argv) {
    int pthrerr, i;
    struct thread_arg *allargs;
    pthread_t *alltids;
    pthread_mutex_t clk_mutex, mutex;
    nblocked = 0;


    /* Get Arguements and override initial variables
     * lam = 0.1
     * mu = 0.2
     * T = 100
     * N = 5
     * */
    while (++i < argc) {
        if (strncmp("-L", argv[i], strlen(argv[i])) == 0)
            lam = atof(argv[++i]);
        else if (strncmp("-M", argv[i], strlen(argv[i])) == 0)
            mu = atof(argv[++i]);
        else if (strncmp("-T", argv[i], strlen(argv[i])) == 0)
            T = atoi(argv[++i]);
        else if (strncmp("-N", argv[i], strlen(argv[i])) == 0)
            N = atoi(argv[++i]);
        else
            fatalerr(argv[i], 0, "Invalid argument\n");
    }
    if (i != argc)
        fatalerr(argv[0], 0, "Odd number of args\n");

    int lchopsticks[N], nblock_c[N];
    int lchairs = N - 1;
    pthread_cond_t lchopsticks_cond[N];


    /* Initializer conditions and mutex */
    pthrerr = pthread_cond_init(&start_line_cond, NULL);
    if (pthrerr != 0)
        fatalerr(argv[0], 0, "Initialization failed\n");
    pthrerr = pthread_cond_init(&clk_cond, NULL);
    if (pthrerr != 0)
        fatalerr(argv[0], 0, "Initialization failed\n");
    pthrerr = pthread_cond_init(&phil_cond, NULL);
    if (pthrerr != 0)
        fatalerr(argv[0], 0, "Initialization failed\n");
    pthrerr = pthread_mutex_init(&clk_mutex, NULL);
    if (pthrerr != 0)
        fatalerr(argv[0], 0, "Initialization failed\n");
    pthrerr = pthread_mutex_init(&mutex, NULL);
    if (pthrerr != 0)
        fatalerr(argv[0], 0, "Initialization failed\n");
    pthrerr = pthread_cond_init(&chairs_cond, NULL);
    if (pthrerr != 0)
        fatalerr(argv[0], 0, "Initialization failed\n");
    for (i = 0; i < N; i++) {
        lchopsticks[i] = 1;
        nblock_c[i] = 0;
        pthrerr = pthread_cond_init(&lchopsticks_cond[i], NULL);
        if (pthrerr != 0)
            fatalerr(argv[0], 0, "Initialization failed\n");
    }
    chopsticks_cond = lchopsticks_cond;
    chopsticks = lchopsticks;
    chairs = &lchairs;
    nblocked_chop = nblock_c;


    /* Create input args for philosophers */
    allargs = (struct thread_arg *)
            malloc((N + 1) * sizeof(struct thread_arg));
    if (allargs == NULL)
        fatalerr(argv[0], 0, "Out of memory\n");
    alltids = (pthread_t *)
            malloc(N * sizeof(pthread_t));
    if (alltids == NULL)
        fatalerr(argv[0], 0, "Out of memory\n");

    allargs[0].seed = 100;
    allargs[0].mutex = &mutex;
    allargs[0].state = THINKING;
    allargs[0].id = 0;

    //create all threads
    for (i = 0; i < N; i++) {
        allargs[i] = allargs[0];
        allargs[i].seed += i;
        allargs[i].id += i;
        pthrerr = pthread_create(alltids + i, NULL, philosopher, allargs + i);
        if (pthrerr != 0)
            fatalerr(argv[0], pthrerr, "Philosopher creation failed\n");
    }

    //call the clock
    clk((void *) (allargs));
    printf("Should not be here\n");
    exit(-1);
}
