#include "node.h"
#include "dv.h"

#define ME 2
#define NEIGHBOR_NUM 3

static struct distance_table dt;
static const int neighbors[] = {0, 1, 3};
static const int costs_to_neighbor[] = {3, 1, 2};
static const int costs_to_nodes_init[] = {3, 1, 0, 2};

void rtinit2() {
    rtinit(ME, &dt, neighbors, NEIGHBOR_NUM, costs_to_nodes_init);
}

void rtupdate2(struct rtpkt *rcvdpkt) {
    rtupdate(ME, &dt, neighbors, NEIGHBOR_NUM, costs_to_neighbor, rcvdpkt);
}
