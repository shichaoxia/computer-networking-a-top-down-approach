#include "dv.h"
#include <stdio.h>
#include <stdlib.h>
#include "node.h"

#define LINKCHANGES 1

int TRACE = 1;
struct event *evlist = NULL; /* the event list */
double clocktime = 0.000;

static float jimsrand(void);

static void insertevent(struct event *p);

void init(void) /* initialize the simulator */
{
    int i;
    float sum, avg;
    struct event *evptr;

    printf("Enter TRACE:");
    scanf("%d", & TRACE);

    srand(9999);              /* init random number generator */
    sum = 0.0;                /* test random number generator for students */
    for (i = 0; i < 1000; i++)
        sum = sum + jimsrand();    /* jimsrand() should be uniform in [0,1] */
    avg = sum / 1000.0;
    if (avg < 0.25 || avg > 0.75) {
        printf("It is likely that random number generation on your machine\n");
        printf("is different from what this emulator expects.  Please take\n");
        printf("a look at the routine jimsrand() in the emulator code. Sorry. \n");
        exit(0);
    }

    clocktime = 0.0;                /* initialize time to 0.0 */
    rtinit0();
    rtinit1();
    rtinit2();
    rtinit3();

    /* initialize future link changes */
    if (LINKCHANGES == 1) {
        evptr = (struct event *) malloc(sizeof(struct event));
        evptr->evtime = 10000.0;
        evptr->evtype = LINK_CHANGE;
        evptr->eventity = -1;
        evptr->rtpktptr = NULL;
        insertevent(evptr);
        evptr = (struct event *) malloc(sizeof(struct event));
        evptr->evtype = LINK_CHANGE;
        evptr->evtime = 20000.0;
        evptr->rtpktptr = NULL;
        insertevent(evptr);
    }
}

void creatertpkt(struct rtpkt *initrtpkt, int srcid, int destid, int mincosts[]) {
    int i;
    initrtpkt->sourceid = srcid;
    initrtpkt->destid = destid;
    for (i = 0; i < 4; i++)
        initrtpkt->mincost[i] = mincosts[i];
}

void print_rtpkt(const struct rtpkt *pkt) {
    printf("rtpkt: ");
    printf("{sourceid: %d", pkt->sourceid);
    printf(", destid: %d", pkt->destid);
    printf(", mincost:");
    for (size_t i = 0; i < 4; i++) {
        printf(" %d", pkt->mincost[i]);
    }
    printf("}\n");
}

void tolayer2(struct rtpkt packet) {
    struct rtpkt *mypktptr;
    struct event *evptr, *q;
    float lastime;
    int i;

    int connectcosts[4][4];

    /* initialize by hand since not all compilers allow array initilization */
    connectcosts[0][0] = 0;
    connectcosts[0][1] = 1;
    connectcosts[0][2] = 3;
    connectcosts[0][3] = 7;
    connectcosts[1][0] = 1;
    connectcosts[1][1] = 0;
    connectcosts[1][2] = 1;
    connectcosts[1][3] = 999;
    connectcosts[2][0] = 3;
    connectcosts[2][1] = 1;
    connectcosts[2][2] = 0;
    connectcosts[2][3] = 2;
    connectcosts[3][0] = 7;
    connectcosts[3][1] = 999;
    connectcosts[3][2] = 2;
    connectcosts[3][3] = 0;

    /* be nice: check if source and destination id's are reasonable */
    if (packet.sourceid < 0 || packet.sourceid > 3) {
        printf("WARNING: illegal source id in your packet, ignoring packet!\n");
        return;
    }
    if (packet.destid < 0 || packet.destid > 3) {
        printf("WARNING: illegal dest id in your packet, ignoring packet!\n");
        return;
    }
    if (packet.sourceid == packet.destid) {
        printf("WARNING: source and destination id's the same, ignoring packet!\n");
        return;
    }
    if (connectcosts[packet.sourceid][packet.destid] == 999) {
        printf("WARNING: source and destination not connected, ignoring packet!\n");
        return;
    }

/* make a copy of the packet student just gave me since he/she may decide */
/* to do something with the packet after we return back to him/her */
    mypktptr = (struct rtpkt *) malloc(sizeof(struct rtpkt));
    mypktptr->sourceid = packet.sourceid;
    mypktptr->destid = packet.destid;
    for (i = 0; i < 4; i++)
        mypktptr->mincost[i] = packet.mincost[i];
    if (TRACE > 2) {
        printf("    TOLAYER2: source: %d, dest: %d\n              costs:",
               mypktptr->sourceid, mypktptr->destid);
        for (i = 0; i < 4; i++)
            printf("%d  ", mypktptr->mincost[i]);
        printf("\n");
    }

/* create future event for arrival of packet at the other side */
    evptr = (struct event *) malloc(sizeof(struct event));
    evptr->evtype = FROM_LAYER2;   /* packet will pop out from layer3 */
    evptr->eventity = packet.destid; /* event occurs at other entity */
    evptr->rtpktptr = mypktptr;       /* save ptr to my copy of packet */

/* finally, compute the arrival time of packet at the other end.
   medium can not reorder, so make sure packet arrives between 1 and 10
   time units after the latest arrival time of packets
   currently in the medium on their way to the destination */
    lastime = clocktime;
    for (q = evlist; q != NULL; q = q->next)
        if ((q->evtype == FROM_LAYER2 && q->eventity == evptr->eventity))
            lastime = q->evtime;
    evptr->evtime = lastime + 2. * jimsrand();


    if (TRACE > 2)
    printf("    TOLAYER2: scheduling arrival on other side\n");
    insertevent(evptr);
}

/****************************************************************************/
/* jimsrand(): return a float in range [0,1].  The routine below is used to */
/* isolate all random number generation in one location.  We assume that the*/
/* system-supplied rand() function return an int in therange [0,mmm]        */
/****************************************************************************/
float jimsrand(void) {
    double mmm = 2147483647;   /* largest int  - MACHINE DEPENDENT!!!!!!!!   */
    float x;                   /* individual students may need to change mmm */
    x = rand() / mmm;            /* x should be uniform in [0,1] */
    return (x);
}

/********************* EVENT HANDLINE ROUTINES *******/
/*  The next set of routines handle the event list   */
/*****************************************************/

static void insertevent(struct event *p) {
    struct event *q, *qold;

    if (TRACE > 3) {
        printf("            INSERTEVENT: time is %lf\n", clocktime);
        printf("            INSERTEVENT: future time will be %lf\n", p->evtime);
    }
    q = evlist;     /* q points to header of list in which p struct inserted */
    if (q == NULL) {   /* list is empty */
        evlist = p;
        p->next = NULL;
        p->prev = NULL;
    } else {
        for (qold = q; q != NULL && p->evtime > q->evtime; q = q->next)
            qold = q;
        if (q == NULL) {   /* end of list */
            qold->next = p;
            p->prev = qold;
            p->next = NULL;
        } else if (q == evlist) { /* front of list */
            p->next = evlist;
            p->prev = NULL;
            p->next->prev = p;
            evlist = p;
        } else {     /* middle of list */
            p->next = q;
            p->prev = q->prev;
            q->prev->next = p;
            q->prev = p;
        }
    }
}

