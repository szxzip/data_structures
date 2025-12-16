#include "header.h"

// 构造函数
Graph::Graph(int vertices)
    : V(vertices)
    , time(0)
{
    adj.resize(V);
    visited.resize(V, false);
    disc.resize(V, -1);
    low.resize(V, -1);
    parent.resize(V, -1);
    articulationPoints.resize(V, false);
}

// 添加边
void Graph::addEdge(int u, int v)
{
    if (u >= 0 && u < V && v >= 0 && v < V && u != v) {
        if (!edgeExists(u, v)) {
            adj[u].push_back(v);
            adj[v].push_back(u);
        }
    }
}

// 删除边
void Graph::removeEdge(int u, int v)
{
    if (u >= 0 && u < V && v >= 0 && v < V) {
        // 从u的邻接表中删除v
        int pos = find(adj[u], v);
        if (pos != -1) {
            adj[u][pos] = adj[u].back();
            adj[u].pop_back();
        }

        // 从v的邻接表中删除u
        pos = find(adj[v], u);
        if (pos != -1) {
            adj[v][pos] = adj[v].back();
            adj[v].pop_back();
        }
    }
}

// 检查边是否存在
bool Graph::edgeExists(int u, int v) const
{
    if (u < 0 || u >= V || v < 0 || v >= V)
        return false;
    return find(adj[u], v) != -1;
}

// 获取边数
int Graph::getEdgeCount() const
{
    int count = 0;
    for (int i = 0; i < V; i++) {
        count += adj[i].size();
    }
    return count / 2; // 无向图，每条边被计算两次
}

// 打印图结构
void Graph::printGraph() const
{
    for (int i = 0; i < V; i++) {
        std::cout << "顶点 " << i << ": ";
        for (size_t j = 0; j < adj[i].size(); j++) {
            std::cout << adj[i][j] << " ";
        }
        std::cout << std::endl;
    }
}

// 改造关节点为非关节点
bool Graph::convertToNonArticulation(int vertex)
{
    if (!isArticulationPoint(vertex)) {
        return false; // 不是关节点，无需改造
    }

    // 找到所有使得该顶点成为关节点的子节点
    for (size_t i = 0; i < adj[vertex].size(); i++) {
        int v = adj[vertex][i];
        // 尝试连接v到vertex的另一个邻居
        for (size_t j = 0; j < adj[vertex].size(); j++) {
            int neighbor = adj[vertex][j];
            if (neighbor != v && !edgeExists(v, neighbor)) {
                addEdge(v, neighbor);
                // 重新计算关节点
                findArticulationPoints();
                if (!isArticulationPoint(vertex)) {
                    return true;
                }
            }
        }
    }

    return false;
}

// 清除图
void Graph::clear()
{
    adj.clear();
    V = 0;
    articulationPoints.clear();
}

// 重置图大小
void Graph::resize(int vertices)
{
    V = vertices;
    adj.resize(V);
    articulationPoints.resize(V, false);
}

// 从控制台输入图
void Graph::inputFromConsole()
{
    std::cout << "请输入顶点数: ";
    std::cin >> V;

    resize(V);

    std::cout << "请输入边数: ";
    int E;
    std::cin >> E;

    std::cout << "请输入" << E << "条边 (格式: u v):" << std::endl;
    for (int i = 0; i < E; i++) {
        int u, v;
        std::cin >> u >> v;
        if (u >= 0 && u < V && v >= 0 && v < V) {
            addEdge(u, v);
        } else {
            std::cout << "无效的边，请重新输入" << std::endl;
            i--;
        }
    }
}
