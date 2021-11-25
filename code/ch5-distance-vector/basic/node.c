#include <stdio.h>
#include <strings.h>
#include <stdlib.h>
#include <stdbool.h>
#include "node.h"
#include "dv.h"

int message_count = 0;

static void send_dt_to_neighbors(int node,
                                 struct distance_table *dt,
                                 const int neighbors[],
                                 int neighbor_num);

static int compare(const void *elem1, const void *elem2);

static void printdt(const struct distance_table *dtptr);

void rtinit(int node,
            struct distance_table *dt,
            const int neighbors[],
            int neighbor_num,
            const int costs_to_nodes_init[]) {
    printf("Router%d init\n", node);

    /*1.初始化自己的距离向量，其余节点初始化为∞*/
    for (int i = 0; i < NODE_NUM; ++i) {
        for (int j = 0; j < NODE_NUM; ++j) {
            dt->costs[i][j] = INFINITY;
        }
    }
    memcpy(dt->costs[node], costs_to_nodes_init, sizeof(dt->costs[node]));
    printf("    current dt is\n");
    printdt(dt);

    /*2.把自己的距离向量发送给邻居*/
    send_dt_to_neighbors(node, dt, neighbors, neighbor_num);
}

void rtupdate(int node,
              struct distance_table *dt,
              const int neighbors[],
              int neighbor_num,
              const int costs_to_neighbor[],
              const struct rtpkt *rcvdpkt) {
    printf("Router%d receive ", node);
    print_rtpkt(rcvdpkt);

    /*1.在距离表中更新收到的距离向量*/
    int sourceid = rcvdpkt->sourceid;
    memcpy(dt->costs[sourceid], rcvdpkt->mincost, sizeof(dt->costs[sourceid]));

    bool dt_updated = false;

    /*2.计算到节点i到距离是否需要更新*/
    for (int i = 0; i < NODE_NUM; i++) {
        /* 0to1 = min{c(0,1)+D1(1), c(0,2)+D2(1), c(0,3)+D3(1)} */

        if (i == node) continue; /*目标是自身的时候，结果是0*/
        printf("    recalculate cost to router%d\n", i);

        /*2.1.重新计算经由所有邻居到i的路径*/
        int current_cost = dt->costs[node][i];
        printf("    current cost is %d\n", current_cost);

        int *candidates = malloc(neighbor_num * sizeof(int));

        for (int j = 0; j < neighbor_num; ++j) {
            int c = costs_to_neighbor[j] + dt->costs[neighbors[j]][i];
            printf("    via %d to %d cost is %d\n", neighbors[j], i, c);
            candidates[j] = c;
        }

        /*2.2.比较是否需要更新*/
        qsort(candidates, neighbor_num, sizeof(int), compare);

        if (candidates[0] != current_cost) {
            printf("    update cost to router%d from %d to %d\n", i, current_cost, candidates[0]);
            dt->costs[node][i] = candidates[0];
            printf("    now dt is\n");
            printdt(dt);
            dt_updated = true;
        } else {
            printf("    no better cost\n");
        }
        free(candidates);
    }
    if (dt_updated) send_dt_to_neighbors(node, dt, neighbors, neighbor_num);
}

static void send_dt_to_neighbors(int node,
                                 struct distance_table *dt,
                                 const int neighbors[],
                                 int neighbor_num) {
    printf("Router%d send ", node);
    for (int i = 0; i < neighbor_num; i++) {
        if (neighbors[i] == node) continue; /*不需要对自己发送*/
        struct rtpkt sndpkt = {};
        creatertpkt(&sndpkt, node, neighbors[i], dt->costs[node]);
        if (i == 0) {
            printf("{%d %d %d %d} to",
                   sndpkt.mincost[0], sndpkt.mincost[1],
                   sndpkt.mincost[2], sndpkt.mincost[3]);
        }
        printf(" router%d", neighbors[i]);
        tolayer2(sndpkt);
        message_count += 1;
    }
    printf("\n");
}

static int compare(const void *elem1, const void *elem2) {
    int f = *((int *) elem1);
    int s = *((int *) elem2);
    if (f > s)
        return 1;
    if (f < s)
        return -1;
    return 0;
}

static void printdt(const struct distance_table *dtptr) {
    printf("                    to            \n");
    printf("          |    0     1     2     3\n");
    printf("      ----|-----------------------\n");
    printf("         0|  %3d   %3d   %3d   %3d\n", dtptr->costs[0][0], dtptr->costs[0][1], dtptr->costs[0][2],
           dtptr->costs[0][3]);
    printf("    from 1|  %3d   %3d   %3d   %3d\n", dtptr->costs[1][0], dtptr->costs[1][1], dtptr->costs[1][2],
           dtptr->costs[1][3]);
    printf("         2|  %3d   %3d   %3d   %3d\n", dtptr->costs[2][0], dtptr->costs[2][1], dtptr->costs[2][2],
           dtptr->costs[2][3]);
    printf("         3|  %3d   %3d   %3d   %3d\n", dtptr->costs[3][0], dtptr->costs[3][1], dtptr->costs[3][2],
           dtptr->costs[3][3]);
}
