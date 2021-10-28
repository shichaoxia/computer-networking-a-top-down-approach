#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <sys/queue.h>
#include "rdt.h"

#define PUBLIC  /* empty */
#define PRIVATE static
#define TIMEOUT 25
#define N 8  /* window size */

struct entry {
  struct pkt packet;
  STAILQ_ENTRY(entry) entries;
};

PRIVATE STAILQ_HEAD(tailhead, entry) head = STAILQ_HEAD_INITIALIZER(head);

PRIVATE int base;
PRIVATE int nextseqnum;
PRIVATE int expectedseqnum;

PRIVATE struct pkt B_sndpkt;

PRIVATE struct pkt make_pkt(int seq, int ack, char payload[]) {
  struct pkt p = {seq, ack, 0, {0}};
  if (payload) memcpy(p.payload, payload, sizeof(p.payload));
  p.checksum = calc_checksum(p);
  return p;
}

PRIVATE bool corrupt(struct pkt p) {
  return p.checksum != calc_checksum(p);
}

PRIVATE bool hasseqnum(struct pkt p, int expt) {
  return p.seqnum == expt;
}

PUBLIC void A_output(struct msg message) {
  printf("\n>>> Layer5 ↓ A, ");
  print_msg(message);
  if (nextseqnum < base + N) {
    struct pkt sndpkt = make_pkt(nextseqnum, 0, message.data);
    printf("    Send ");
    print_pkt(sndpkt);
    struct entry *elem = malloc(sizeof(struct entry));
    elem->packet = sndpkt;
    STAILQ_INSERT_TAIL(&head, elem, entries);
    tolayer3(0, sndpkt);
    if (base == nextseqnum) starttimer(0, TIMEOUT);
    nextseqnum += 1;
  } else {
    printf("    Refuse data\n");
  }
}

PUBLIC void A_input(struct pkt packet) {
  printf("\n>>> Layer3 ↑ A, ");
  print_pkt(packet);
  if (corrupt(packet)) {
    printf("    Ignore corrupt packet\n");
    return;
  }
  int acknum = packet.acknum;
  printf("*** Ack packet: %d\n", acknum);
  if (acknum >= 20) {
    printf(">>> 20 messages were successfully transfered from sender to receiver\n");
    exit(0);
  }
  for (int i=acknum-base+1; i>0; --i) {
    struct entry *elem  = STAILQ_FIRST(&head);
    STAILQ_REMOVE_HEAD(&head, entries);
    free(elem);
  }
  base = acknum + 1;

  if (base == nextseqnum) {
    stoptimer(0);
  } else {
    stoptimer(0);
    starttimer(0, TIMEOUT);
  }

}

PUBLIC void A_timerinterrupt() {
  printf("\n>>> A timeout\n");
  starttimer(0, TIMEOUT);
  struct entry *elem;
  STAILQ_FOREACH(elem, &head, entries) {
    tolayer3(0, elem->packet);
    printf("    Resend ");
    print_pkt(elem->packet);
  }
}

PUBLIC void A_init() {
  base = 1;
  nextseqnum = 1;
}

PUBLIC void B_output(struct msg message) {
}

PUBLIC void B_input(struct pkt packet) {
  printf("\n>>> Layer3 ↑ B, ");
  print_pkt(packet);
  if (corrupt(packet)) {
    printf("    Ignore corrupt packet\n");
    return;
  }
  if (!hasseqnum(packet, expectedseqnum)) {
    printf("    Unexpected packet, resend last ACK, ");
    print_pkt(B_sndpkt);
    tolayer3(1, B_sndpkt);
    return;
  }
  tolayer5(1, packet.payload);
  B_sndpkt = make_pkt(0, expectedseqnum, NULL);
  tolayer3(1, B_sndpkt);
  printf("    B send ACK ");
  print_pkt(B_sndpkt);
  expectedseqnum += 1;
}

PUBLIC void B_timerinterrupt() {
}

PUBLIC void B_init() {
  expectedseqnum = 1;
  B_sndpkt = make_pkt(0, 0, NULL);
}
