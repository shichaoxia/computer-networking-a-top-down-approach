#include "rdt.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define PUBLIC /* empty */
#define PRIVATE static
#define TIMEOUT 25

enum stat {WAIT_0, WAIT_ACK_0, WAIT_1, WAIT_ACK_1};
enum stat A_state;
enum stat B_state;

PRIVATE struct pkt unacked_packet = {0};
PRIVATE int received = 0;

PUBLIC void A_output(struct msg message) {
  printf("\n>>> layer5 ↓ A, ");
  print_msg(message);
  struct pkt sndpkt = {0};

  memcpy(sndpkt.payload, message.data, sizeof(sndpkt.payload));
  switch (A_state) {
  case WAIT_0:
    sndpkt.seqnum = 0;
    sndpkt.checksum = calc_checksum(sndpkt);
    tolayer3(0, sndpkt);
    starttimer(0, TIMEOUT);
    unacked_packet = sndpkt;
    A_state = WAIT_ACK_0;
    printf("    A state: WAIT_0\n");
    printf("    A ↓ layer3, ");
    print_pkt(sndpkt);
    printf("    A new state: WAIT_ACK_0\n");
    break;
  case WAIT_ACK_0:
    printf("    A state: WAIT_ACK_0, ignore\n");
    break;
  case WAIT_1:
    sndpkt.seqnum = 1;
    sndpkt.checksum = calc_checksum(sndpkt);
    tolayer3(0, sndpkt);
    starttimer(0, TIMEOUT);
    unacked_packet = sndpkt;
    A_state = WAIT_ACK_1;
    printf("    A state: WAIT_1\n");
    printf("    A ↓ layer3, ");
    print_pkt(sndpkt);
    printf("    A new state: WAIT_ACK_1\n");
    break;
  case WAIT_ACK_1:
    printf("    A state: WAIT_ACK_1, ignore\n");
    break;
  }
}

PUBLIC void A_input(struct pkt packet) {
  printf("\n>>> layer3 ↑ A, ");
  print_pkt(packet);
  switch (A_state) {
  case WAIT_0:
    printf("    A state: WAIT_0, ignore\n");
    break;
  case WAIT_ACK_0:
    printf("    A state: WAIT_ACK_0\n");
    if (packet.checksum != calc_checksum(packet) ||
        packet.acknum != 0) {
      printf("    corrupt or ack1, ignore\n");
      return;
    }
    stoptimer(0);
    A_state = WAIT_1;
    printf("    acked\n");
    printf("    A new state: WAIT_1\n");
    received += 1;
    printf("    A收到B对第%d个分组的确认\n", received);
    if (received >= 10) exit(0);
    break;
  case WAIT_1:
    printf("    A state: WAIT_1, ignore\n");
    break;
  case WAIT_ACK_1:
    printf("    A state: WAIT_ACK_1\n");
    if (packet.checksum != calc_checksum(packet) ||
        packet.acknum != 1) {
      printf("    corrupt or ack0, ignore\n");
      return;
    }
    stoptimer(0);
    A_state = WAIT_0;
    printf("    acked\n");
    printf("    A new state: WAIT_0\n");
    received += 1;
    printf("    A收到B对第%d个分组的确认\n", received);
    if (received >= 10) exit(0);
    break;
  }
}

PUBLIC void A_timerinterrupt() {
  printf("\n>>> A timeout\n");
  switch (A_state) {
  case WAIT_0:
    printf("    A state: WAIT_0, ignore\n");
    break;
  case WAIT_ACK_0:
    printf("    A state: WAIT_ACK_0\n");
    printf("    A ↓ layer3, ");
    print_pkt(unacked_packet);
    tolayer3(0, unacked_packet);
    starttimer(0, TIMEOUT);
    break;
  case WAIT_1:
    printf("    A state: WAIT_1, ignore\n");
    break;
  case WAIT_ACK_1:
    printf("    A state: WAIT_ACK_1\n");
    printf("    A ↓ layer3, ");
    print_pkt(unacked_packet);
    tolayer3(0, unacked_packet);
    starttimer(0, TIMEOUT);
    break;
  }
}

PUBLIC void A_init() {
  A_state = WAIT_0;
}

PUBLIC void B_output(struct msg message) {
}

PUBLIC void B_input(struct pkt packet) {
  printf("\n>>> layer3 ↑ B, ");
  print_pkt(packet);
  struct pkt sndpkt = {0};
  switch (B_state) {
  case WAIT_0:
    printf("    B state: WAIT_0\n");
    if ((packet.checksum != calc_checksum(packet)) ||
        (packet.seqnum == 1)) {
      sndpkt.acknum = 1;
      sndpkt.checksum = calc_checksum(sndpkt);
      tolayer3(1, sndpkt);
      printf("    corrupt or seq1, send ACK1\n");
      printf("    B ↓ layer3, ");
      print_pkt(sndpkt);
    } else if ((packet.checksum == calc_checksum(packet)) &&
               (packet.seqnum == 0)) {
      tolayer5(1, packet.payload);
      sndpkt.acknum = 0;
      sndpkt.checksum = calc_checksum(sndpkt);
      tolayer3(1, sndpkt);
      B_state = WAIT_1;
      printf("    B ↑ layer5, send ACK0\n");
      printf("    B ↓ layer3, ");
      print_pkt(sndpkt);
    }
    break;
  case WAIT_1:
    printf("    B state: WAIT_1\n");
    if ((packet.checksum != calc_checksum(packet)) ||
        (packet.seqnum == 0)) {
      sndpkt.acknum = 0;
      sndpkt.checksum = calc_checksum(sndpkt);
      tolayer3(1, sndpkt);
      printf("    corrupt or seq0, send ACK0\n");
      printf("    B ↓ layer3, ");
      print_pkt(sndpkt);
    } else if ((packet.checksum == calc_checksum(packet)) &&
               (packet.seqnum == 1)) {
      tolayer5(1, packet.payload);
      sndpkt.acknum = 1;
      sndpkt.checksum = calc_checksum(sndpkt);
      tolayer3(1, sndpkt);
      B_state = WAIT_0;
      printf("    B ↑ layer5, send ACK1\n");
      printf("    B ↓ layer3, ");
      print_pkt(sndpkt);
    }
    break;
  default:
    break;
  }
}

PUBLIC void B_timerinterrupt() {
}

PUBLIC void B_init() {
  B_state = WAIT_0;
}
