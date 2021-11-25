#include <stdio.h>
#include <stdlib.h>
#include "dv.h"
#include "node.h"

int main() {
    struct event *eventptr;

    init();

    while (1) {

        eventptr = evlist;            /* get next event to simulate */
        if (eventptr == NULL)
            goto terminate;
        evlist = evlist->next;        /* remove this event from event list */
        if (evlist != NULL)
            evlist->prev = NULL;
        if (TRACE > 1) {
            printf("MAIN: rcv event, t=%.3f, at %d",
                   eventptr->evtime, eventptr->eventity);
            if (eventptr->evtype == FROM_LAYER2) {
                printf(" src:%2d,", eventptr->rtpktptr->sourceid);
                printf(" dest:%2d,", eventptr->rtpktptr->destid);
                printf(" contents: %3d %3d %3d %3d\n",
                       eventptr->rtpktptr->mincost[0], eventptr->rtpktptr->mincost[1],
                       eventptr->rtpktptr->mincost[2], eventptr->rtpktptr->mincost[3]);
            } else {
                printf("\n");
            }
        }
        clocktime = eventptr->evtime;    /* update time to next event time */
        if (eventptr->evtype == FROM_LAYER2) {
            if (eventptr->eventity == 0)
                rtupdate0(eventptr->rtpktptr);
            else if (eventptr->eventity == 1)
                rtupdate1(eventptr->rtpktptr);
            else if (eventptr->eventity == 2)
                rtupdate2(eventptr->rtpktptr);
            else if (eventptr->eventity == 3)
                rtupdate3(eventptr->rtpktptr);
            else {
                printf("Panic: unknown event entity\n");
                exit(0);
            }
        } else if (eventptr->evtype == LINK_CHANGE) {
            if (clocktime < 10001.0) {
                linkhandler0(1, 20);
                linkhandler1(0, 20);
            } else {
                linkhandler0(1, 1);
                linkhandler1(0, 1);
            }
        } else {
            printf("Panic: unknown event type\n");
            exit(0);
        }
        if (eventptr->evtype == FROM_LAYER2)
            free(eventptr->rtpktptr);        /* free memory for packet, if any */
        free(eventptr);                    /* free memory for event struct   */
    }

    terminate:
    printf("\nTotal of %d messages sent", message_count);
    printf("\ndt0:\n");
    printdt(&dt0, 1);
    printf("\ndt1:\n");
    printdt(&dt1, 1);
    printf("\ndt2:\n");
    printdt(&dt2, 1);
    printf("\ndt3:\n");
    printdt(&dt3, 1);
    printf("\nSimulator terminated at t=%f, no packets in medium\n", clocktime);
}
