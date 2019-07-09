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
int nblocked;            /* The number of threads blocked */
int nThink, nHungry, nEating = 0;
enum STATES {
    THINKING, HUNGRY, EATING
};
int *chopsticks, *chairs;
pthread_cond_t start_line_cond, clk_cond, chairs_cond, *chopsticks_cond;

int get_chair() {
    int i;
    for (i = 0; i < N - 1; ++i) {
        if (*(chairs + i) == 1) {
            return i;
        }
    }
    return -1;
}

/***********************************************************************
                         P H I L O S O P H E R
************************************************************************/
void *philosopher(void *vptr) {
    int pthrerr, chair;            /* creating a new thread               */
    struct thread_arg *ptr;
    enum STATES state = THINKING;

    ptr = (struct thread_arg *) vptr;

    while (1) {
        // Block on start_line
        pthrerr = pthread_mutex_lock(ptr->clk_mutex);
        if (pthrerr != 0)
            fatalerr("Client", pthrerr, "Mutex lock failed\n");
        nblocked++;
        if (nblocked == N) {            /* cond_signal never returns an error code */
            pthrerr = pthread_cond_signal(&clk_cond);
            if (pthrerr != 0)
                fatalerr("Client", pthrerr, "Condition signal failed\n");
        }
        printf("Thread id: 0x%02x, waiting thread, STATE: %d\n", (unsigned) (pthread_self()), state);
        pthrerr = pthread_cond_wait(&start_line_cond, ptr->clk_mutex);
        if (pthrerr != 0)
            fatalerr("Client", pthrerr, "Condition wait failed\n");
        pthrerr = pthread_mutex_unlock(ptr->clk_mutex);
        if (pthrerr != 0)
            fatalerr("Client", pthrerr, "Mutext unlock failed\n");
        printf("Thread id: 0x%02x, working, STATE: %d\n", (unsigned) (pthread_self()), state);
        pthrerr = pthread_mutex_lock(ptr->philosopher_mutex);
        if (pthrerr != 0)
            fatalerr("Client", pthrerr, "Mutex lock failed\n");
        // THINK, HUNGRY or EATING
        if (state == THINKING) {
            nThink++;
            // if less than lamda then he switch to hungry state
            // need to go EATING when the chopsticks become available.
            if (rand0_1(&(ptr->seed)) < lam) {
                state = HUNGRY;
            }
        }else if (state == HUNGRY) {
            nHungry++;

            chair = get_chair();
            if (chair < 0) {
                // Wait for a chair
                while (chair < 0) {
                    printf("Thread id: 0x%02x, waiting for chair\n", (unsigned) (pthread_self()));
                    pthrerr = pthread_cond_wait(&chairs_cond, ptr->philosopher_mutex);
                    if (pthrerr != 0)
                        fatalerr("Client", pthrerr, "Condition wait failed\n");
                    chair = get_chair();
                }
            }
            // grab the chair
            *(chairs + chair) = 0;
            if (!*(chopsticks + *(ptr->chops)) || !*(chopsticks + *(ptr->chops + 1))) {
                printf("C1, C2: %d, %d\n", *(chopsticks + *(ptr->chops)), *(chopsticks + *(ptr->chops + 1)));
                // Wait for a chair
                while (!*(chopsticks + *(ptr->chops)) || !*(chopsticks + *(ptr->chops + 1))) {
                    if (!*(chopsticks + *(ptr->chops))) {
                        printf("Thread id: 0x%02x, waiting for chop\n", (unsigned) (pthread_self()));
                        pthrerr = pthread_cond_wait((chopsticks_cond + *(ptr->chops)), ptr->philosopher_mutex);
                        if (pthrerr != 0)
                            fatalerr("Client", pthrerr, "Condition wait failed\n");
                    }
                    if (!*(chopsticks + *(ptr->chops + 1))) {
                        printf("Thread id: 0x%02x, waiting for chop\n", (unsigned) (pthread_self()));
                        pthrerr = pthread_cond_wait((chopsticks_cond + *(ptr->chops + 1)), ptr->philosopher_mutex);
                        if (pthrerr != 0)
                            fatalerr("Client", pthrerr, "Condition wait failed\n");
                    }
                }
            }
            *(chopsticks + ptr->chops[0]) = 0;
            *(chopsticks + ptr->chops[1]) = 0;
            state = EATING;
        }else if (state == EATING) {
            nEating++;
        }

        // If less than mu, then we switch to THINKING.
        *(chopsticks + ptr->chops[0]) = 1;
        *(chopsticks + ptr->chops[1]) = 1;
        *(chairs + chair) = 1;
        pthrerr = pthread_cond_signal((chopsticks_cond + *(ptr->chops)));
        if (pthrerr != 0)
            fatalerr("Clock", pthrerr, "Condition b/cast failed\n");
        pthrerr = pthread_cond_signal((chopsticks_cond + *(ptr->chops + 1)));
        if (pthrerr != 0)
            fatalerr("Clock", pthrerr, "Condition b/cast failed\n");
        pthrerr = pthread_cond_broadcast(&chairs_cond);
        if (pthrerr != 0)
            fatalerr("Clock", pthrerr, "Condition b/cast failed\n");
        printf("Thread id: 0x%02x, done clock, STATE: %d\n", (unsigned) (pthread_self()), state);
        if (rand0_1(&(ptr->seed)) < mu) {
            state = THINKING;
        } else {
            state = HUNGRY;
        }


        pthrerr = pthread_mutex_unlock(ptr->philosopher_mutex);
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
    pthrerr = pthread_mutex_lock(ptr->clk_mutex);
    if (pthrerr != 0)
        fatalerr("Clock", pthrerr, "Mutex lock failed\n");
    for (tick = 0; tick <= T; tick++) {
        while (nblocked < N) {
            pthrerr = pthread_cond_wait(&clk_cond, ptr->clk_mutex);
            if (pthrerr != 0)
                fatalerr("Clock", 0, "Condition wait failed\n");
        }
        nblocked = 0;
        printf("Clock BD: %d\n", tick);
        pthrerr = pthread_cond_broadcast(&start_line_cond);
        if (pthrerr != 0)
            fatalerr("Clock", 0, "Condition b/cast failed\n");

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
    pthread_mutex_t clk_mutex, philosopher_mutex;

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
/*    printf("i: %d, L: %f, M: %f, T: %d, N: %d\n", i, lam, mu, T, N);
    printf("i: %d, argc: %d\n", i, argc);*/
    if (i != argc)
        fatalerr(argv[0], 0, "Odd number of args\n");

    nblocked = 0;
    int lchopsticks[N], lchairs[N - 1];
    pthread_cond_t lchopsticks_cond[N];

    /* Initializer conditions and mutex */
    pthrerr = pthread_cond_init(&start_line_cond, NULL);
    if (pthrerr != 0)
        fatalerr(argv[0], 0, "Initialization failed\n");
    pthrerr = pthread_cond_init(&clk_cond, NULL);
    if (pthrerr != 0)
        fatalerr(argv[0], 0, "Initialization failed\n");
    pthrerr = pthread_mutex_init(&clk_mutex, NULL);
    if (pthrerr != 0)
        fatalerr(argv[0], 0, "Initialization failed\n");
    pthrerr = pthread_mutex_init(&philosopher_mutex, NULL);
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
        if (i < N - 1) {
            lchairs[i] = 1;
        }
    }
//    printf("Working \n");
    chopsticks_cond = lchopsticks_cond;
    chopsticks = lchopsticks;
    chairs = lchairs;
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
    allargs[0].clk_mutex = &clk_mutex;
    allargs[0].philosopher_mutex = &philosopher_mutex;

    //create all threads
    for (i = 0; i < N; i++) {
        allargs[i] = allargs[0];
        allargs[i].seed += i;
        allargs[i].chops[0] = i % N;
        allargs[i].chops[1] = (i + 1) % N;
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
