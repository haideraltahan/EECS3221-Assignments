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
pthread_cond_t start_line_cond, clk_cond;
pthread_mutex_t mutex;

int get_chair(int *chairs){
    int i, in;
    for (i = 0; i < N - 1; ++i) {
        if (*(chairs + i) == 1){
            return i;
        }
    }
    return -1;
}

int* get_chopstick(int *c1, int *c2){
    int ans[2] = {0, 0};
    if(c1 && c2){
        ans[0] = 1;
        ans[1] = 1;
    }else if(c1){
        ans[0] = 1;
    }else if(c2){
        ans[1] = 1;
    }
    return ans;
}

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
        pthrerr = pthread_mutex_lock(&mutex);
        if (pthrerr != 0)
            fatalerr("Client", pthrerr, "Mutex lock failed\n");
        nblocked++;
        if (nblocked == N) {            /* cond_signal never returns an error code */
            pthrerr = pthread_cond_signal(&clk_cond);
            if (pthrerr != 0)
                fatalerr("Client", 0, "Condition signal failed\n");
        }
        printf("Thread id: %d, start clock, STATE: %d\n", pthread_self(),state);
        pthrerr = pthread_cond_wait(&start_line_cond, &mutex);
        if (pthrerr != 0)
            fatalerr("Client", 0, "Condition wait failed\n");
        printf("Thread id: %d, working, STATE: %d\n", pthread_self(),state);
        // THINK, HUNGRY or EATING
        if (state == THINKING){
            nThink += 1;
            // if less than lamda then he switch to hungry state
            // need to go EATING when the chopsticks become available.
            if ( rand0_1(&(ptr->seed)) < lam) {
                state = HUNGRY;
                int chair = get_chair(ptr->chairs);
                if (chair < 0){
                    // Wait for a chair
                    while (chair < 0){
                        pthrerr = pthread_cond_wait(&chair, &mutex);
                        if (pthrerr != 0)
                            fatalerr("Client", 0, "Condition wait failed\n");
                        chair = get_chair(ptr->chairs);
                    }
                }
                // grab the chair
                ptr->chairs[chair] = 1;
            }
        }else if(state == HUNGRY){
            nHungry += 1;
            int *chopsticks = get_chopstick(ptr->chopsticks[ptr->chops[0]], ptr->chopsticks[ptr->chops[1]]);
            if(!chopsticks[0] && !chopsticks[1]){
                // Wait for a chair
                while (!chopsticks[0] && !chopsticks[1]){
                    if (!ptr->chopsticks[ptr->chops[0]]) {
                        pthrerr = pthread_cond_wait(&ptr->chopsticks_cond[ptr->chops[0]], &mutex);
                        if (pthrerr != 0)
                            fatalerr("Client", 0, "Condition wait failed\n");
                    }
                    if (!ptr->chopsticks[ptr->chops[1]]) {
                        pthrerr = pthread_cond_wait(&ptr->chopsticks_cond[ptr->chops[1]], &mutex);
                        if (pthrerr != 0)
                            fatalerr("Client", 0, "Condition wait failed\n");
                    }
                    chopsticks = get_chopstick(ptr->chopsticks[ptr->chops[0]], ptr->chopsticks[ptr->chops[1]]);
                }
            }
            ptr->chopsticks[ptr->chops[0]] = 0;
            ptr->chopsticks[ptr->chops[1]] = 0;
            state = EATING;
        }else if(state == EATING){
            nEating += 1;
            if(rand0_1(&(ptr->seed)) < mu) {
                // If less than mu, then we switch to THINKING.
                ptr->chopsticks[ptr->chops[0]] = 1;
                ptr->chopsticks[ptr->chops[1]] = 1;
                pthrerr = pthread_cond_broadcast(&ptr->chopsticks_cond[ptr->chops[0]]);
                if (pthrerr != 0)
                    fatalerr("Clock", 0, "Condition b/cast failed\n");
                pthrerr = pthread_cond_broadcast(&ptr->chopsticks_cond[ptr->chops[1]]);
                if (pthrerr != 0)
                    fatalerr("Clock", 0, "Condition b/cast failed\n");
                state = THINKING;
            }
        }
        printf("Thread id: %d, done clock, STATE: %d\n", pthread_self(), state);
        pthrerr = pthread_mutex_unlock(&mutex);
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
    pthrerr = pthread_mutex_lock(&mutex);
    if (pthrerr != 0)
        fatalerr("Clock", pthrerr, "Mutex lock failed\n");
    for (tick = 0; tick <= T; tick++) {
        printf("Clock: %d\n", tick);
        while (nblocked < N) {
            pthrerr = pthread_cond_wait(&clk_cond, &mutex);
            if (pthrerr != 0)
                fatalerr("Clock", 0, "Condition wait failed\n");
        }
        nblocked = 0;
        pthrerr = pthread_cond_broadcast(&start_line_cond);
        if (pthrerr != 0)
            fatalerr("Clock", 0, "Condition b/cast failed\n");
    }
    printf("Average Thinking Time:    %6.2f\n", (float) nThink / (float) (T*N));
    printf("Average Hungry Time:    %6.2f\n", (float) nHungry / (float) (T*N));
    printf("Average Eating Time:    %6.2f\n", (float) nEating / (float) (T*N));
    exit(0);
}

/***********************************************************************
                               M A I N
************************************************************************/
int main(int argc, char **argv) {
    int pthrerr, i;
    struct thread_arg *allargs;
    pthread_t *alltids;

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
    printf("i: %d, L: %f, M: %f, T: %d, N: %d\n", i, lam, mu, T, N);
    printf("i: %d, argc: %d\n", i, argc);
    if (i != argc)
        fatalerr(argv[0], 0, "Odd number of args\n");

    nblocked = 0;
    int chopsticks[N], chairs[N - 1];
    pthread_cond_t chopsticks_cond[N], chairs_cond[N - 1];

    /* Initializer conditions and mutex */
    pthrerr = pthread_cond_init(&start_line_cond, NULL);
    if (pthrerr != 0)
        fatalerr(argv[0], 0, "Initialization failed\n");
    pthrerr = pthread_cond_init(&clk_cond, NULL);
    if (pthrerr != 0)
        fatalerr(argv[0], 0, "Initialization failed\n");
    pthrerr = pthread_mutex_init(&mutex, NULL);
    if (pthrerr != 0)
        fatalerr(argv[0], 0, "Initialization failed\n");
    for(i=0; i < N; i++){
        chopsticks[i] = 1;
        pthrerr = pthread_cond_init(&chopsticks_cond[i], NULL);
        if (pthrerr != 0)
            fatalerr(argv[0], 0, "Initialization failed\n");
        if(i < N - 1){
            chairs[i] = 1;
            pthrerr = pthread_cond_init(&chairs_cond[i], NULL);
            if (pthrerr != 0)
                fatalerr(argv[0], 0, "Initialization failed\n");
        }
    }
    printf("Working \n");

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
    allargs[0].chopsticks = chopsticks;
    allargs[0].chairs = chairs;
    allargs[0].chopsticks_cond = chopsticks_cond;
    allargs[0].chairs_cond = chairs_cond;

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
