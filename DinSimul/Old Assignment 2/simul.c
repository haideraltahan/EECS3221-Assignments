/***********************************************************************/
/**      Author: Minas Spetsakis                                      **/
/**        Date: Nov. 10 2016                                         **/
/** Description: Solution for Assgn. II                               **/
/***********************************************************************/

#include <stdio.h>
#include <sys/time.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>

#include "queue.h"
#include "args.h"
#include "error.h"

int njobs,            /* number of jobs ever created */
        ttlserv,            /* total service offered by servers */
        ttlqlen;            /* the total qlength */
int nblocked;            /* The number of threads blocked */

/***********************************************************************
                           r a n d 0 _ 1
************************************************************************/
double rand0_1(unsigned int *seedp) {
    double f;
    /* We use the re-entrant version of rand */
    f = (double) rand_r(seedp);
    return f / (double) RAND_MAX;
}


/***********************************************************************
                             C L I E N T
************************************************************************/

void *client(void *vptr) {
    unsigned int seed;
    int pthrerr;
    struct thread_arg *ptr;

    ptr = (struct thread_arg *) vptr;

    while (1) {
        if (rand0_1(&(ptr->seed)) < ptr->lam) {
            push_q(ptr->q);    /* the queue has its own mutext protection */
            pthrerr = pthread_mutex_lock(ptr->statex);
            if (pthrerr != 0)
                fatalerr("Client", pthrerr, "Mutex lock failed\n");
            njobs++;
            pthrerr = pthread_mutex_unlock(ptr->statex);
            if (pthrerr != 0)
                fatalerr("Client", pthrerr, "Mutex unlock failed\n");
        }
        pthrerr = pthread_mutex_lock(ptr->blocktex);
        if (pthrerr != 0)
            fatalerr("Client", pthrerr, "Mutex lock failed\n");
        nblocked++;
        if (nblocked == (ptr->nserver + ptr->nclient)) {            /* cond_signal never returns an error code */
            pthrerr = pthread_cond_signal(ptr->clkblockcond);
            if (pthrerr != 0)
                fatalerr("Client", 0, "Condition signal failed\n");
        }
        pthrerr = pthread_cond_wait(ptr->thrblockcond, ptr->blocktex);
        if (pthrerr != 0)
            fatalerr("Client", 0, "Condition wait failed\n");

        pthrerr = pthread_mutex_unlock(ptr->blocktex);
        if (pthrerr != 0)
            fatalerr("Client", pthrerr, "Mutext unlock failed\n");
    }
    return NULL;

}

/***********************************************************************
                             S E R V E R
************************************************************************/

void *server(void *vptr) {
    int busy;
    int pthrerr;
    struct thread_arg *ptr;

    ptr = (struct thread_arg *) vptr;

    busy = 0;
    while (1) {
        if (busy == 1) {
            pthrerr = pthread_mutex_lock(ptr->statex);
            if (pthrerr != 0)
                fatalerr("Server", pthrerr, "Mutex lock failed\n");
            ttlserv++;
            pthrerr = pthread_mutex_unlock(ptr->statex);
            if (pthrerr != 0)
                fatalerr("Server", pthrerr, "Mutex unlock failed\n");
            if (rand0_1(&(ptr->seed)) < ptr->mu) {
                busy = 0;
            }
        } else if (safepop_q(ptr->q)) {
            busy = 1;
        }
        pthrerr = pthread_mutex_lock(ptr->blocktex);
        if (pthrerr != 0)
            fatalerr("Server", pthrerr, "Mutext lock failed\n");
        nblocked++;
        if (nblocked == (ptr->nserver + ptr->nclient)) {
            pthrerr = pthread_cond_signal(ptr->clkblockcond);
            if (pthrerr != 0)
                fatalerr("Server", 0, "Condition signal failed\n");
        }
        pthrerr = pthread_cond_wait(ptr->thrblockcond, ptr->blocktex);
        if (pthrerr != 0)
            fatalerr("Server", 0, "Condition wait failed\n");
        pthrerr = pthread_mutex_unlock(ptr->blocktex);
        if (pthrerr != 0)
            fatalerr("Server", pthrerr, "Mutext unlock failed\n");
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

    pthrerr = pthread_mutex_lock(ptr->blocktex);
    if (pthrerr != 0)
        fatalerr("Clock", pthrerr, "Mutex lock failed\n");
    for (tick = 0; tick < ptr->nticks; tick++) {
        while (nblocked < (ptr->nserver + ptr->nclient)) {
            pthrerr = pthread_cond_wait(ptr->clkblockcond, ptr->blocktex);
            if (pthrerr != 0)
                fatalerr("Clock", 0, "Condition wait failed\n");
        }
        nblocked = 0;
        ttlqlen += size_q(ptr->q);
        pthrerr = pthread_cond_broadcast(ptr->thrblockcond);
        if (pthrerr != 0)
            fatalerr("Clock", 0, "Condition b/cast failed\n");
    }
    printf("Average waiting time:    %f\n", (float) ttlqlen / (float) njobs);
    printf("Average execution time:  %f\n", (float) ttlserv / (float) njobs);
    printf("Average turnaround time: %f\n", (float) ttlqlen / (float) njobs +
                                            (float) ttlserv / (float) njobs);
    printf("Average queue length: %f\n", (float) ttlqlen / (float) ptr->nticks);
    printf("Average interarrival time time: %f\n", (float) ptr->nticks / (float) njobs);
    /* Here we die with mutex locked and everyone else asleep */
    exit(0);
}

int main(int argc, char **argv) {
    int pthrerr, i;
    int nserver, nclient, nticks;
    float lam, mu;

    pthread_t server_tid, client_tid;
    pthread_cond_t sthrblockcond, sclkblockcond;
    pthread_mutex_t sblocktex, sstatex;
    struct thread_arg *allargs;
    pthread_t *alltids;


    ttlserv = 0;
    ttlqlen = 0;
    nblocked = 0;
    njobs = 0;
    pthrerr = pthread_cond_init(&sthrblockcond, NULL);
    if (pthrerr != 0)        /* Initializers never return an error code */
        fatalerr(argv[0], 0, "Initialization failed\n");
    pthrerr = pthread_cond_init(&sclkblockcond, NULL);
    if (pthrerr != 0)
        fatalerr(argv[0], 0, "Initialization failed\n");
    pthrerr = pthread_mutex_init(&sblocktex, NULL);
    if (pthrerr != 0)
        fatalerr(argv[0], 0, "Initialization failed\n");
    pthrerr = pthread_mutex_init(&sstatex, NULL);
    if (pthrerr != 0)
        fatalerr(argv[0], 0, "Initialization failed\n");

    nserver = 2;
    nclient = 2;
    lam = 0.005;
    mu = 0.01;
    nticks = 1000;
    i = 1;
    while (i < argc - 1) {
        if (strncmp("--lambda", argv[i], strlen(argv[i])) == 0)
            lam = atof(argv[++i]);
        else if (strncmp("--mu", argv[i], strlen(argv[i])) == 0)
            mu = atof(argv[++i]);
        else if (strncmp("--servers", argv[i], strlen(argv[i])) == 0)
            nserver = atoi(argv[++i]);
        else if (strncmp("--clients", argv[i], strlen(argv[i])) == 0)
            nclient = atoi(argv[++i]);
        else if (strncmp("--ticks", argv[i], strlen(argv[i])) == 0)
            nticks = atoi(argv[++i]);
        else
            fatalerr(argv[i], 0, "Invalid argument\n");
        i++;
    }
    if (i != argc)
        fatalerr(argv[0], 0, "Odd number of args\n");

    allargs = (struct thread_arg *)
            malloc((nserver + nclient + 1) * sizeof(struct thread_arg));
    if (allargs == NULL)
        fatalerr(argv[0], 0, "Out of memory\n");
    alltids = (pthread_t *)
            malloc((nserver + nclient) * sizeof(pthread_t));
    if (alltids == NULL)
        fatalerr(argv[0], 0, "Out of memory\n");

    allargs[0].nserver = nserver;
    allargs[0].nclient = nclient;
    allargs[0].nticks = nticks;
    allargs[0].q = mk_queue();
    allargs[0].lam = lam;
    allargs[0].mu = mu;
    allargs[0].thrblockcond = &sthrblockcond;
    allargs[0].clkblockcond = &sclkblockcond;
    allargs[0].blocktex = &sblocktex;
    allargs[0].statex = &sstatex;
    allargs[0].seed = 100;
    for (i = 0; i < nserver; i++) {
        allargs[i] = allargs[0];
        allargs[i].seed += i;
        pthrerr = pthread_create(alltids + i, NULL, server, allargs + i);
        if (pthrerr != 0)
            fatalerr(argv[0], pthrerr, "Server creation failed\n");
    }
    for (i = nserver; i < nserver + nclient; i++) {
        allargs[i] = allargs[0];
        allargs[i].seed += i;
        pthrerr = pthread_create(alltids + i, NULL, client, allargs + i);
        if (pthrerr != 0)
            fatalerr(argv[0], pthrerr, "Client creation failed\n");
    }
    allargs[nserver + nclient] = allargs[0];
    allargs[nserver + nclient].seed += nserver + nclient;
    clk((void *) (allargs + (nserver + nclient)));
    printf("Should not be here\n");
    exit(-1);
}
