#include "node.h"
#include "dv.h"

#define ME 1
#define NEIGHBOR_NUM 2

static struct distance_table dt;
static const int neighbors[] = {0, 2};
static const int costs_to_neighbor[] = {1, 1};
static const int costs_to_nodes_init[] = {1, 0, 1, INFINITY};

void rtinit1() {
    rtinit(ME, &dt, neighbors, NEIGHBOR_NUM, costs_to_nodes_init);
}

void rtupdate1(struct rtpkt *rcvdpkt) {
    rtupdate(ME, &dt, neighbors, NEIGHBOR_NUM, costs_to_neighbor, rcvdpkt);
}

void linkhandler1(int linkid, int newcost) {}