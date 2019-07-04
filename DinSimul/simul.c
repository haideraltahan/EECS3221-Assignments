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

#include "args.h"
#include "error.h"

int N = 5, T = 100;
double lam = 0.1, mu = 0.2;
int nblocked;            /* The number of threads blocked */
int nThink, nHungry, nEating = 0;
enum STATES{THINKING, HUNGRY, EATING};
int chopsticks[] = {1,1,1,1,1};

/***********************************************************************
                         P H I L O S O P H E R
************************************************************************/
void *philosopher(void *vptr) {
    int pthrerr;            /* creating a new thread               */
    struct thread_arg *ptr;
    enum STATES state = THINKING;

    ptr = (struct thread_arg *) vptr;

    while (1) {
        // Block on start_line
        pthrerr = pthread_mutex_lock(ptr->mutex);
        if (pthrerr != 0)
            fatalerr("Client", pthrerr, "Mutex lock failed\n");
        nblocked++;
        if (nblocked == ptr->N) {            /* cond_signal never returns an error code */
            pthrerr = pthread_cond_signal(ptr->chair);
            if (pthrerr != 0)
                fatalerr("Client", 0, "Condition signal failed\n");
        }
        pthrerr = pthread_cond_wait(ptr->start_line, ptr->mutex);
        if (pthrerr != 0)
            fatalerr("Client", 0, "Condition wait failed\n");
        // THINK, HUNGRY or EATING
        if (state == THINKING){
            nThink += 1;
            // if less than lamda then he switch to hungry state
            // need to go EATING when the chopsticks become available.
            if ( rand0_1(&(ptr->seed)) < ptr->lam) {
                state = HUNGRY;
            }
        }else if(state == HUNGRY){
            nHungry += 1;
            // wait for chopstick
            if (chopsticks[ptr->seed % N] && chopsticks[(ptr->seed + 1) % N]){
                chopsticks[ptr->seed % N] = chopsticks[(ptr->seed + 1) % N] = 0;
                state = EATING;
            }
        }else if(state == EATING && rand0_1(&(ptr->seed)) < ptr->mu){
            nEating += 1;
            // If less than mu, then we switch to THINKING.
            chopsticks[ptr->seed % N] = chopsticks[(ptr->seed + 1) % N] = 1;
            state = THINKING;
        }
        pthrerr = pthread_mutex_unlock(ptr->mutex);
        if (pthrerr != 0)
            fatalerr("Client", pthrerr, "Mutex unlock failed\n");
    }
}

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
    for (tick = 0; tick < ptr->T; tick++) {
        while (nblocked < ptr-> N - 1) {
            pthrerr = pthread_cond_wait(ptr->chair, ptr->mutex);
            if (pthrerr != 0)
                fatalerr("Clock", 0, "Condition wait failed\n");
        }
        nblocked = 0;
        pthrerr = pthread_cond_broadcast(ptr->start_line);
        if (pthrerr != 0)
            fatalerr("Clock", 0, "Condition b/cast failed\n");
    }
    printf("Average Thinking Time:    %f\n", (float) nThink / (float) N);
    printf("Average Hungry Time:    %f\n", (float) nHungry / (float) N);
    printf("Average Eating Time:    %f\n", (float) nEating / (float) N);
    exit(0);
}

/***********************************************************************
                               M A I N
************************************************************************/
int main(int argc, char **argv) {
    int pthrerr, i;

    pthread_cond_t start_line, chair;
    pthread_mutex_t mutex;
    struct thread_arg *allargs;
    pthread_t *alltids;

    nblocked = 0;

    /* Initializer conditions and mutex */
    pthrerr = pthread_cond_init(&start_line, NULL);
    if (pthrerr != 0)
        fatalerr(argv[0], 0, "Initialization failed\n");
    pthrerr = pthread_cond_init(&chair, NULL);
    if (pthrerr != 0)
        fatalerr(argv[0], 0, "Initialization failed\n");
    pthrerr = pthread_mutex_init(&mutex, NULL);
    if (pthrerr != 0)
        fatalerr(argv[0], 0, "Initialization failed\n");

    /* Create args for philosophers */
    allargs = (struct thread_arg *)
            malloc((N + 1) * sizeof(struct thread_arg));
    if (allargs == NULL)
        fatalerr(argv[0], 0, "Out of memory\n");
    alltids = (pthread_t *)
            malloc(N * sizeof(pthread_t));
    if (alltids == NULL)
        fatalerr(argv[0], 0, "Out of memory\n");

    allargs[0].N = N;
    allargs[0].T = T;
    allargs[0].lam = lam;
    allargs[0].mu = mu;
    allargs[0].mutex = &mutex;
    allargs[0].start_line = &start_line;
    allargs[0].chair = &chair;
    allargs[0].seed = 100;

    //create all threads
    for (i = 0; i < N; i++) {
        allargs[i] = allargs[0];
        allargs[i].seed += i;
        pthrerr = pthread_create(alltids + i, NULL, philosopher, allargs + i);
        if (pthrerr != 0)
            fatalerr(argv[0], pthrerr, "Philosopher creation failed\n");
    }

    //Wakes all the children up
    allargs[N] = allargs[0];
    allargs[N].seed += N;
    clk((void *) (allargs + (N)));
    printf("Should not be here\n");
    exit(-1);
}
