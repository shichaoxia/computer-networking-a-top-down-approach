#ifndef dv_h
#define dv_h

#define FROM_LAYER2 2
#define LINK_CHANGE 1

struct rtpkt
{
    int sourceid;   /* id of sending router sending this pkt */
    int destid;     /* id of router to which pkt being sent
                         (must be an immediate neighbor) */
    int mincost[4]; /* min cost to node 0 ... 3 */
};

struct event
{
    float evtime;           /* event time */
    int evtype;             /* event type code */
    int eventity;           /* entity where event occurs */
    struct rtpkt *rtpktptr; /* ptr to packet (if any) assoc w/ this event */
    struct event *prev;
    struct event *next;
};

extern struct event *evlist;
extern int TRACE;
//extern int YES;
//extern int NO;
extern double clocktime;

void init(void);
void tolayer2(struct rtpkt packet);
void creatertpkt(struct rtpkt *initrtpkt, int srcid, int destid, int mincosts[]);
void print_rtpkt(const struct rtpkt *pkt);

#endif
