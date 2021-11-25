#include "node.h"
#include "dv.h"

#define ME 1
#define NEIGHBOR_NUM 2

struct distance_table dt1;
static const int neighbors[] = {0, 2};
static int costs_to_neighbor[] = {1, 1};
static const int costs_to_nodes_init[] = {1, 0, 1, INFINITY};

void rtinit1() {
    rtinit(ME, &dt1, neighbors, NEIGHBOR_NUM, costs_to_nodes_init);
}

void rtupdate1(struct rtpkt *rcvdpkt) {
    rtupdate(ME, &dt1, neighbors, NEIGHBOR_NUM, costs_to_neighbor, rcvdpkt);
}

void linkhandler1(int linkid, int newcost) {
    linkhandler(ME, &dt1, neighbors, NEIGHBOR_NUM, costs_to_neighbor, linkid, newcost);
}