#include <stdio.h>
#include <strings.h>
#include <stdlib.h>
#include <stdbool.h>
#include "node.h"
#include "dv.h"

#define PRIVATE static
#define PUBLIC

int message_count = 0;

PRIVATE void
send_dt_to_neighbors(int node, struct distance_table *dt, const int neighbors[], int neighbor_num) {
    printf("    router%d send ", node);
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

PRIVATE int
compare(const void *elem1, const void *elem2) {
    int f = *((int *) elem1);
    int s = *((int *) elem2);
    if (f > s)
        return 1;
    if (f < s)
        return -1;
    return 0;
}

PUBLIC void
printdt(const struct distance_table *dtptr, int level) {
    /*加上level层的缩进（4个空格），0表示不缩进*/
    char *prefix = malloc((level * 4 + 1) * sizeof(char));
    prefix[sizeof(prefix) - 1] = '\0';
    while (level > 0) {
        strcat(prefix, "    ");
        level -= 1;
    }
    printf("%s                to            \n", prefix);
    printf("%s      |    0     1     2     3\n", prefix);
    printf("%s  ----|-----------------------\n", prefix);
    printf("%s     0|  %3d   %3d   %3d   %3d\n", prefix, dtptr->costs[0][0], dtptr->costs[0][1], dtptr->costs[0][2],
           dtptr->costs[0][3]);
    printf("%sfrom 1|  %3d   %3d   %3d   %3d\n", prefix, dtptr->costs[1][0], dtptr->costs[1][1], dtptr->costs[1][2],
           dtptr->costs[1][3]);
    printf("%s     2|  %3d   %3d   %3d   %3d\n", prefix, dtptr->costs[2][0], dtptr->costs[2][1], dtptr->costs[2][2],
           dtptr->costs[2][3]);
    printf("%s     3|  %3d   %3d   %3d   %3d\n", prefix, dtptr->costs[3][0], dtptr->costs[3][1], dtptr->costs[3][2],
           dtptr->costs[3][3]);
    free(prefix);
}

PRIVATE size_t
find_index(const int a[], size_t size, int value) {
    size_t index = 0;

    while (index < size && a[index] != value) ++index;

    return (index == size ? -1 : index);
}

PRIVATE bool
recalculate(int node, struct distance_table *dt, const int *neighbors, int neighbor_num, const int *costs_to_neighbor) {
    bool dt_updated;

    for (int i = 0; i < NODE_NUM; i++) {
        /*到目标节点i*/
        /* 0to1 = min{c(0,1)+D1(1), c(0,2)+D2(1), c(0,3)+D3(1)} */

        if (i == node) continue; /*目标是自身的时候，结果是0*/
        printf("    recalculate cost to router%d\n", i);

        /*2.1.重新计算经由所有邻居到i的路径*/
        int current_cost = dt->costs[node][i];
        printf("        current cost is %d\n", current_cost);

        int *candidates = malloc(neighbor_num * sizeof(int));
        for (int j = 0; j < neighbor_num; ++j) {
            int c = costs_to_neighbor[j] + dt->costs[neighbors[j]][i];
            printf("        via %d to %d cost is %d\n", neighbors[j], i, c);
            candidates[j] = c;
        }

        /*2.2.比较是否需要更新*/
        qsort(candidates, neighbor_num, sizeof(int), compare);

        if (candidates[0] != current_cost) {
            /*只要不同于原来的距离就更新，在链路距离发生变化时，这个新计算出的结果可能比原来的大*/

            printf("        update cost to router%d from %d to %d\n",
                   i, current_cost, candidates[0]);

            dt->costs[node][i] = candidates[0];
            printf("        now dt1 is\n");
            printdt(dt, 2);

            dt_updated = true;
        } else {
            printf("        no better cost\n");
        }
        free(candidates);
    }
    return dt_updated;
}

PUBLIC void
rtinit(int node, struct distance_table *dt, const int neighbors[], int neighbor_num, const int costs_to_nodes_init[]) {
    printf("Router%d init\n", node);

    /*1.初始化自己的距离向量，其余节点初始化为∞*/
    for (int i = 0; i < NODE_NUM; ++i) {
        for (int j = 0; j < NODE_NUM; ++j) {
            dt->costs[i][j] = INFINITY;
        }
    }
    memcpy(dt->costs[node], costs_to_nodes_init, sizeof(dt->costs[node]));
    printf("    current dt1 is\n");
    printdt(dt, 1);

    /*2.把自己的距离向量发送给邻居*/
    send_dt_to_neighbors(node, dt, neighbors, neighbor_num);
}

PUBLIC void
rtupdate(int node, struct distance_table *dt, const int neighbors[], int neighbor_num, const int costs_to_neighbor[],
         const struct rtpkt *rcvdpkt) {
    printf("Router%d receive ", node);
    print_rtpkt(rcvdpkt);

    /*1.在距离表中更新收到的距离向量*/
    memcpy(dt->costs[rcvdpkt->sourceid], rcvdpkt->mincost, sizeof(rcvdpkt->mincost));

    /*2.计算经邻居到其他所有节点的距离是否需要更新*/
    bool dt_updated = recalculate(node, dt, neighbors, neighbor_num, costs_to_neighbor);

    /*3.如果更新了，向邻居发送距离向量*/
    if (dt_updated) send_dt_to_neighbors(node, dt, neighbors, neighbor_num);
}

PUBLIC void
linkhandler(int node, struct distance_table *dt, const int neighbors[], int neighbor_num, int costs_to_neighbor[],
            int linkid, int newcost) {
    printf("Router%d to router%d's cost changes to %d\n", node, linkid, newcost);

    size_t neighbor_index = find_index(neighbors, neighbor_num, linkid);

    costs_to_neighbor[neighbor_index] = newcost;

    bool dt_updated = recalculate(node, dt, neighbors, neighbor_num, costs_to_neighbor);

    if (dt_updated) send_dt_to_neighbors(node, dt, neighbors, neighbor_num);
}
