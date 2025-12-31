#include "header.h"

// 构造函数：初始化一个具有指定顶点数的图
Graph::Graph(int vertices)
    : V(vertices) // 顶点数
    , time(0) // DFS 时间戳，用于追踪顶点访问顺序
{
    adj.resize(V); // 调整邻接表大小为顶点数
    visited.resize(V, false); // 初始化所有顶点为未访问
    disc.resize(V, -1); // 初始化所有顶点的发现时间为 -1（未访问）
    low.resize(V, -1); // 初始化所有顶点的 low 值为 -1
    parent.resize(V, -1); // 初始化所有顶点的父节点为 -1（无父节点）
    articulationPoints.resize(V, false); // 初始化所有顶点为非关节点
    cout << "Graph 构造函数：完成！" << endl;
}

// 添加边
void Graph::addEdge(int u, int v)
{
    if (u >= 0 && u < V && v >= 0 && v < V && u != v) {
        // 如果边不存在，才进行添加，避免重复边
        if (!edgeExists(u, v)) {
            adj[u].push_back(v);
            adj[v].push_back(u);
        }
    }
    cout << "添加边：完成！" << endl;
}

// 删除边
void Graph::removeEdge(int u, int v)
{
    if (u >= 0 && u < V && v >= 0 && v < V) {
        // 从 u 的邻接表中删除 v
        int pos = my_find(adj[u], v); // 查找 v 在 u 的邻接表中的位置（my_find：自定义模板函数）
        if (pos != -1) { // 如果找到
            // 将最后一个元素移到当前位置，然后弹出最后一个元素
            adj[u][pos] = adj[u].back();
            adj[u].pop_back();
        }

        // 从 v 的邻接表中删除 u
        pos = my_find(adj[v], u);
        if (pos != -1) {
            adj[v][pos] = adj[v].back();
            adj[v].pop_back();
        }
    }
    cout << "删除边：完成！" << endl;
}

// 检查边是否存在
bool Graph::edgeExists(int u, int v) const
{
    if (u < 0 || u >= V || v < 0 || v >= V) {
        cout << "检查边是否存在：不存在" << endl;
        return false; // 无效顶点，边不存在
    }
    // 在 u 的邻接表中查找 v，如果找到则边存在
    cout << "检查边是否存在：存在" << endl;
    return my_find(adj[u], v) != -1;
}

// 获取边数
int Graph::getEdgeCount() const
{
    int count = 0;
    for (int i = 0; i < V; i++) {
        count += adj[i].size(); // 累加每个顶点的邻接表大小
    }
    cout << "获取边数：" << count / 2 << endl;
    return count / 2; // 无向图每条边在邻接表中出现两次
}

// 打印图结构
void Graph::printGraph() const
{
    for (int i = 0; i < V; i++) {
        cout << "顶点 " << i << ": ";
        for (size_t j = 0; j < adj[i].size(); j++) {
            cout << adj[i][j] << " ";
        }
    }
    cout << "打印图结构：完成！" << endl;
}

// 改造关节点为非关节点：尝试通过添加额外边将指定顶点变为非关节点
bool Graph::convertToNonArticulation(int vertex)
{
    // 如果该顶点本身不是关节点，则无需改造
    if (!isArticulationPoint(vertex)) {
        cout << "改造关节点 " << vertex << " ：失败！" << endl;
        return false;
    }

    // 遍历该顶点的所有邻接顶点 v
    for (size_t i = 0; i < adj[vertex].size(); i++) {
        int v = adj[vertex][i];
        // 对于每个邻接顶点 v，尝试将其连接到 vertex 的另一个邻居 neighbor
        for (size_t j = 0; j < adj[vertex].size(); j++) {
            int neighbor = adj[vertex][j];
            // 如果 neighbor 不是 v 且 v 和 neighbor 之间没有边，则添加新边
            if (neighbor != v && !edgeExists(v, neighbor)) {
                addEdge(v, neighbor);
                // 重新计算图中的所有关节点
                findArticulationPoints();
                // 检查 vertex 是否仍然是关节点
                if (!isArticulationPoint(vertex)) {
                    cout << "改造关节点 " << vertex << " ：成功！" << endl;
                    return true;
                }
            }
        }
    }
    cout << "改造关节点 " << vertex << " ：失败！" << endl;
    return false;
}
