#include "node.h"
#include "dv.h"

#define ME 0                              /*指明自身是哪个节点*/
#define NEIGHBOR_NUM 3                    /*邻居节点数量*/

static struct distance_table dt;                         /*距离表*/
static const int neighbors[] = {1, 2, 3};                /*所有邻居节点*/
static const int costs_to_neighbor[] = {1, 3, 7};        /*到相应邻居节点的距离*/
static const int costs_to_nodes_init[] = {0, 1, 3, 7};   /*初始化所用的到所有节点的距离，按照0、1、2、3的顺序*/

void rtinit0() {
    rtinit(ME, &dt, neighbors, NEIGHBOR_NUM, costs_to_nodes_init);
}

void rtupdate0(struct rtpkt *rcvdpkt) {
    rtupdate(ME, &dt, neighbors, NEIGHBOR_NUM, costs_to_neighbor, rcvdpkt);
}

void linkhandler0(int linkid, int newcost) {}
