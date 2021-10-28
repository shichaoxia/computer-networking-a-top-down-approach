#ifndef ADT_H
#define ADT_H
/* change to 1 if you're doing extra credit */
/* and write a routine called B_output */
#define BIDIRECTIONAL 0

/* a "msg" is the data unit passed from layer 5 (teachers code) to layer  */
/* 4 (students' code).  It contains the data (characters) to be delivered */
/* to layer 5 via the students transport level protocol entities.         */

struct msg {
  char data[20];
};


/* a packet is the data unit passed from layer 4 (students code) to layer */
/* 3 (teachers code).  Note the pre-defined packet structure, which all   */
/* students must follow. */
struct pkt {
   int seqnum;
   int acknum;
   int checksum;
   char payload[20];
};

/* called from layer 5, passed the data to be sent to other side */
void A_output(struct msg message);

/* called from layer 3, when a packet arrives for layer 4 */
void A_input(struct pkt packet);

/* called when A's timer goes off */
void A_timerinterrupt();

/* the following routine will be called once (only) before any other */
/* entity A routines are called. You can use it to do any initialization */
void A_init();

/* need be completed only for extra credit */
void B_output(struct msg message);

/* called from layer 3, when a packet arrives for layer 4 at B*/
void B_input(struct pkt packet);

/* called when B's timer goes off */
void B_timerinterrupt();

/* the following rouytine will be called once (only) before any other */
/* entity B routines are called. You can use it to do any initialization */
void B_init();


/* Student-callable ROUTINES */
void stoptimer(int AorB);
void starttimer(int AorB, float increment);
void tolayer3(int AorB, struct pkt packet);
void tolayer5(int AorB, char datasent[20]);

/* Helper */
void print_pkt(struct pkt packet);
void print_msg(struct msg message);
int calc_checksum(struct pkt packet);

#endif
