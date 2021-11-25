#ifndef node_h
#define node_h

#define INFINITY 999
#define NODE_NUM 4

struct rtpkt;

struct distance_table {
    int costs[4][4];
};

extern int message_count; /*计数发送了多少消息*/

/**
 * 初始化节点的距离表，发送其距离向量给邻居
 * @param node 节点
 * @param dt 距离表
 * @param neighbors 邻居
 * @param neighbor_num 邻居数量
 * @param costs_to_nodes_init 其他节点的初始化距离
 */
void rtinit(int node,
            struct distance_table *dt,
            const int neighbors[],
            int neighbor_num,
            const int costs_to_nodes_init[]);

/**
 * 收到距离向量后更新自己的距离表，发送更新的距离向量给邻居
 * @param node 节点
 * @param dt 距离表
 * @param neighbors 邻居
 * @param neighbor_num 邻居的数量
 * @param costs_to_neighbor 到邻居的距离
 * @param rcvdpkt 收到的距离向量
 */
void rtupdate(int node,
              struct distance_table *dt,
              const int neighbors[],
              int neighbor_num,
              const int costs_to_neighbor[],
              const struct rtpkt *rcvdpkt);

void rtinit0(void);

void rtupdate0(struct rtpkt *rcvdpkt);

void linkhandler0(int linkid, int newcost);

void rtinit1(void);

void rtupdate1(struct rtpkt *rcvdpkt);

void linkhandler1(int linkid, int newcost);

void rtinit2(void);

void rtupdate2(struct rtpkt *rcvdpkt);

void rtinit3(void);

void rtupdate3(struct rtpkt *rcvdpkt);

#endif
