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


/***********************************************************************
                         P H I L O S O P H E R
************************************************************************/
void *philosopher(void *vptr) {
    unsigned int seed;        /* This is called from main without    */
    int pthrerr;            /* creating a new thread               */
    struct thread_arg *ptr;

    ptr = (struct thread_arg *) vptr;
    pthread_mutex_lock(ptr->mutex);
    pthread_cond_wait(ptr->start_line, ptr->mutex);
    pthread_mutex_unlock(ptr->mutex);

    while (1) {
        //Do Something
        break;
    }
    return NULL;

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
        while (nblocked < ptr->N) {
            pthrerr = pthread_cond_wait(ptr->chair, ptr->mutex);
            if (pthrerr != 0)
                fatalerr("Clock", 0, "Condition wait failed\n");
        }
        nblocked = 0;
        pthrerr = pthread_cond_broadcast(ptr->chair);
        if (pthrerr != 0)
            fatalerr("Clock", 0, "Condition b/cast failed\n");
    }
/*    printf("Average waiting time:    %f\n", (float) ttlqlen / (float) njobs);
    printf("Average execution time:  %f\n", (float) ttlserv / (float) njobs);
    printf("Average turnaround time: %f\n", (float) ttlqlen / (float) njobs +
                                            (float) ttlserv / (float) njobs);
    printf("Average queue length: %f\n", (float) ttlqlen / (float) ptr->nticks);
    printf("Average interarrival time time: %f\n", (float) ptr->nticks / (float) njobs);*/
    /* Here we die with mutex locked and everyone else asleep */
    exit(0);
}

/***********************************************************************
                               M A I N
************************************************************************/
int main(int argc, char **argv) {
    int pthrerr, i;
    int nphilosophers, nticks;

    pthread_t philosopher_id;
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
    if (pthrerr!=0)
        fatalerr(argv[0], 0,"Initialization failed\n");
    pthrerr = pthread_mutex_init(&mutex, NULL);
    if (pthrerr!=0)
        fatalerr(argv[0], 0,"Initialization failed\n");

    /* Create args for philosophers */
    allargs = (struct thread_arg *)
            malloc((N + 1) * sizeof(struct thread_arg));
    if (allargs == NULL)
        fatalerr(argv[0], 0, "Out of memory\n");
    alltids = (pthread_t *)
            malloc((N) * sizeof(pthread_t));
    if (alltids == NULL)
        fatalerr(argv[0], 0, "Out of memory\n");

    allargs[0].N = N;
    allargs[0].T = T;
    allargs[0].lam = lam;
    allargs[0].mu = mu;
    allargs[0].mutex = &mutex;
    allargs[0].start_line = &start_line;
    allargs[0].chair = &chair;

    //create all threads
    for (i = 0; i < N; i++) {
        allargs[i] = allargs[0];
        pthrerr = pthread_create(alltids + i, NULL, philosopher, allargs + i);
        if (pthrerr != 0)
            fatalerr(argv[0], pthrerr, "Philosopher creation failed\n");
    }

    //Wakes all the children up
    pthrerr = pthread_cond_broadcast(&start_line);
    if (pthrerr != 0)
        fatalerr(argv[0], pthrerr, "Condition b/cast failed\n");
    printf("Should not be here\n");
    exit(-1);
}
