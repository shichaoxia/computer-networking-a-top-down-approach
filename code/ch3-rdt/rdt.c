#include "rdt.h"
#include <stdio.h>

void print_pkt(struct pkt packet) {
  printf("Packet: {seqnum: %d, acknum: %d, checksum: %d, payload: %.20s}\n",
         packet.seqnum,
         packet.acknum,
         packet.checksum,
         packet.payload);
}

void print_msg(struct msg message) {
  printf("Message: {data: %.20s}\n", message.data);
}

int calc_checksum(struct pkt packet) {
  int sum = 0;
  for (int i = 0; i < 20; ++i) {
    sum += packet.payload[i];
  }
  return sum + packet.seqnum + packet.acknum;
}
