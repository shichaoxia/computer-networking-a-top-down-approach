#include "node.h"
#include "dv.h"

#define ME 3
#define NEIGHBOR_NUM 2

static struct distance_table dt;
static const int neighbors[] = {0, 2};
static const int costs_to_neighbor[] = {7, 2};
static const int costs_to_nodes_init[] = {7, INFINITY, 2, 0};

void rtinit3() {
    rtinit(ME, &dt, neighbors, NEIGHBOR_NUM, costs_to_nodes_init);
}

void rtupdate3(struct rtpkt *rcvdpkt) {
    rtupdate(ME, &dt, neighbors, NEIGHBOR_NUM, costs_to_neighbor, rcvdpkt);
}
