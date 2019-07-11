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
int nblocked, nWaiting;            /* The number of threads blocked */
int nThink, nHungry, nEating = 0;
int *chopsticks, *chairs;
pthread_cond_t start_line_cond, clk_cond, phil_cond, chairs_cond, *chopsticks_cond;

/***********************************************************************
                         P H I L O S O P H E R
************************************************************************/
void *philosopher(void *vptr) {
    int pthrerr, chair;            /* creating a new thread               */
    struct thread_arg *ptr;

    ptr = (struct thread_arg *) vptr;

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
        // Block on start_line
        pthrerr = pthread_mutex_lock(ptr->mutex);
        if (pthrerr != 0)
            fatalerr("Philosopher", pthrerr, "Mutex lock failed\n");
        nblocked++;
        if (nblocked + nWaiting == N) {            /* cond_signal never returns an error code */
            pthrerr = pthread_cond_signal(&clk_cond);
            if (pthrerr != 0)
                fatalerr("Philosopher", pthrerr, "Condition signal failed\n");
        }
        pthrerr = pthread_cond_wait(&phil_cond, ptr->mutex);
        if (pthrerr != 0)
            fatalerr("Philosopher", pthrerr, "Condition wait failed\n");

        // THINK, HUNGRY or EATING
        if (ptr->state == EATING) {
            if (rand0_1(&(ptr->seed)) < mu) {
                ptr->state = THINKING;
                *(chopsticks + (chair % N)) = 1;
                *(chopsticks + ((chair + 1) % N)) = 1;
                ++*chairs;
                pthrerr = pthread_cond_signal((chopsticks_cond + (chair % N)));
                if (pthrerr != 0)
                    fatalerr("Philosopher", pthrerr, "Condition b/cast failed\n");
                pthrerr = pthread_cond_signal((chopsticks_cond + ((chair + 1) % N)));
                if (pthrerr != 0)
                    fatalerr("Philosopher", pthrerr, "Condition b/cast failed\n");
                pthrerr = pthread_cond_broadcast(&chairs_cond);
                if (pthrerr != 0)
                    fatalerr("Philosopher", pthrerr, "Condition b/cast failed\n");
            }
        } else {
            // if less than lamda then he switch to hungry state
            // need to go EATING when the chopsticks become available.
            if (rand0_1(&(ptr->seed)) < lam) {
                ptr->state = HUNGRY;
                nWaiting++;
                // Wait for a chair
                while (*chairs <= 0) {
                    if (nblocked + nWaiting == N) {            /* cond_signal never returns an error code */
                        pthrerr = pthread_cond_signal(&clk_cond);
                        if (pthrerr != 0)
                            fatalerr("Philosopher", pthrerr, "Condition signal failed\n");
                    }
                    pthrerr = pthread_cond_wait(&chairs_cond, ptr->mutex);
                    if (pthrerr != 0)
                        fatalerr("Philosopher", pthrerr, "Conditionf wait failed\n");
                }
                nWaiting--;
                // grab the chair
                --*chairs;
                chair = *chairs;

                nWaiting++;
                // Wait for a chair
                while (!*(chopsticks + (chair % N)) || !*(chopsticks + ((chair + 1) % N))) {
                    if (nblocked + nWaiting == N) {            /* cond_signal never returns an error code */
                        pthrerr = pthread_cond_signal(&clk_cond);
                        if (pthrerr != 0)
                            fatalerr("Philosopher", pthrerr, "Condition signal failed\n");
                    }
                    if (!*(chopsticks + (chair % N))) {
                        pthrerr = pthread_cond_wait((chopsticks_cond + (chair % N)), ptr->mutex);
                        if (pthrerr != 0)
                            fatalerr("Philosopher", pthrerr, "Conditionf wait failed\n");
                        *(chopsticks + (chair % N)) = 0;
                    }
                    if (!*(chopsticks + ((chair + 1) % N))) {
                        pthrerr = pthread_cond_wait((chopsticks_cond + ((chair + 1) % N)), ptr->mutex);
                        if (pthrerr != 0)
                            fatalerr("Philosopher", pthrerr, "Condition wait failed\n");
                        *(chopsticks + ((chair + 1) % N)) = 0;
                    }
                }
                nWaiting--;
                ptr->state = EATING;
            }
        }

        pthrerr = pthread_mutex_unlock(ptr->mutex);
        if (pthrerr != 0)
            fatalerr("Philosopher", pthrerr, "Mutex unlock failed\n");
    }
}

/*
 * for 1..T
     *  lock mutex
     *  wait for all phil to block on phil_cond
     *  do stats or print
     *  wake up phils
     *  unlock mutex
     *
     *
 * keep track of the number of phil in nBlock and nWaiting. The addition should be N
 * before getting a chair or chopstick make sure that you are the last phil
 * If you are the last phil
 * */

/***********************************************************************
                                C L K
************************************************************************/
void *clk(void *vptr) {
    int tick;
    int pthrerr;
    struct thread_arg *ptr;

    ptr = (struct thread_arg *) vptr;
    pthrerr = pthread_mutex_lock(ptr->mutex);
    if (pthrerr != 0)
        fatalerr("Clock", pthrerr, "Mutex lock failed\n");
    while (nblocked < N) {
        pthrerr = pthread_cond_wait(&clk_cond, ptr->mutex);
        if (pthrerr != 0)
            fatalerr("Clock", 0, "Condition wait failed\n");
    }
    nblocked = 0;
    //Wakes all the children up
    pthrerr = pthread_cond_broadcast(&start_line_cond);
    if (pthrerr != 0)
        fatalerr("Clock", pthrerr, "Mutex unlock failed\n");
    pthrerr = pthread_mutex_unlock(ptr->mutex);
    if (pthrerr != 0)
        fatalerr("Clock", pthrerr, "Mutex unlock failed\n");
    for (tick = 0; tick < T; tick++) {
        pthrerr = pthread_mutex_lock(ptr->mutex);
        if (pthrerr != 0)
            fatalerr("Clock", pthrerr, "Mutex lock failed\n");
        while (nblocked + nWaiting < N) {
            pthrerr = pthread_cond_wait(&clk_cond, ptr->mutex);
            if (pthrerr != 0)
                fatalerr("Clock", 0, "Condition wait failed\n");
        }
        // Do stats
        for (int i = 0; i < N; i++) {
            printf("%d ", (ptr + i)->state);
            if ((ptr + i)->state == EATING){
                nEating++;
            }else if((ptr + i)->state == HUNGRY){
                nHungry++;
            }else{
                nThink++;
            }
        }
        printf("\n");
        nblocked = 0;
        pthrerr = pthread_cond_broadcast(&phil_cond);
        if (pthrerr != 0)
            fatalerr("Clock", pthrerr, "Mutex unlock failed\n");
        pthrerr = pthread_mutex_unlock(ptr->mutex);
        if (pthrerr != 0)
            fatalerr("Clock", pthrerr, "Mutex unlock failed\n");
    }
    printf("Average Thinking Time: %6.2f\n", (float) nThink / (float) (T * N));
    printf("Average Hungry Time: %6.2f\n", (float) nHungry / (float) (T * N));
    printf("Average Eating Time: %6.2f\n", (float) nEating / (float) (T * N));
    exit(0);
}

/***********************************************************************
                               M A I N
************************************************************************/
int main(int argc, char **argv) {
    int pthrerr, i;
    struct thread_arg *allargs;
    pthread_t *alltids;
    pthread_mutex_t clk_mutex, mutex;

    /* Get Arguements */
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

    nblocked = 0;
    int lchopsticks[N];
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
        pthrerr = pthread_cond_init(&lchopsticks_cond[i], NULL);
        if (pthrerr != 0)
            fatalerr(argv[0], 0, "Initialization failed\n");
    }
    /*printf("Working \n");*/
    chopsticks_cond = lchopsticks_cond;
    chopsticks = lchopsticks;
    chairs = &lchairs;
    /* Create args for philosophers */
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

    //create all threads
    for (i = 0; i < N; i++) {
        allargs[i] = allargs[0];
        allargs[i].seed += i;
        pthrerr = pthread_create(alltids + i, NULL, philosopher, allargs + i);
        if (pthrerr != 0)
            fatalerr(argv[0], pthrerr, "Philosopher creation failed\n");
    }

    // Call the clock
    clk((void *) (allargs));
    printf("Should not be here\n");
    exit(-1);
}
