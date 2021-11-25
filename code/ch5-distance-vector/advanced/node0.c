#include "node.h"
#include "dv.h"

/*指明自身是哪个节点*/
#define ME 0
/*邻居节点数量*/
#define NEIGHBOR_NUM 3
/*距离表*/
struct distance_table dt0;
/*所有邻居节点*/
static const int neighbors[] = {1, 2, 3};
/*到相应邻居节点的距离*/
static int costs_to_neighbor[] = {1, 3, 7};
/*初始化所用的到所有节点的距离，按照0、1、2、3的顺序*/
static const int costs_to_nodes_init[] = {0, 1, 3, 7};

void rtinit0() {
    rtinit(ME, &dt0, neighbors, NEIGHBOR_NUM, costs_to_nodes_init);
}

void rtupdate0(struct rtpkt *rcvdpkt) {
    rtupdate(ME, &dt0, neighbors, NEIGHBOR_NUM, costs_to_neighbor, rcvdpkt);
}

void linkhandler0(int linkid, int newcost) {
    linkhandler(ME, &dt0, neighbors, NEIGHBOR_NUM, costs_to_neighbor, linkid, newcost);
}
