#include "graph_articulation.h"

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

void Graph::addEdge(int u, int v)
{
    if (u >= 0 && u < V && v >= 0 && v < V && u != v) {
        // 检查边是否已存在
        if (!edgeExists(u, v)) {
            adj[u].push_back(v);
            adj[v].push_back(u);
        }
    }
}

void Graph::removeEdge(int u, int v)
{
    if (u >= 0 && u < V && v >= 0 && v < V) {
        adj[u].erase(std::remove(adj[u].begin(), adj[u].end(), v), adj[u].end());
        adj[v].erase(std::remove(adj[v].begin(), adj[v].end(), u), adj[v].end());
    }
}

bool Graph::edgeExists(int u, int v) const
{
    if (u < 0 || u >= V || v < 0 || v >= V)
        return false;
    return std::find(adj[u].begin(), adj[u].end(), v) != adj[u].end();
}

void Graph::DFS(int u)
{
    int children = 0;
    visited[u] = true;
    disc[u] = low[u] = ++time;

    for (int v : adj[u]) {
        if (!visited[v]) {
            children++;
            parent[v] = u;
            DFS(v);

            low[u] = std::min(low[u], low[v]);

            // 规则1: u是根节点且有多个子节点
            if (parent[u] == -1 && children > 1)
                articulationPoints[u] = true;

            // 规则2: u不是根节点且low[v] >= disc[u]
            if (parent[u] != -1 && low[v] >= disc[u])
                articulationPoints[u] = true;
        } else if (v != parent[u]) {
            low[u] = std::min(low[u], disc[v]);
        }
    }
}

void Graph::findArticulationPoints()
{
    // 重置数据
    std::fill(visited.begin(), visited.end(), false);
    std::fill(disc.begin(), disc.end(), -1);
    std::fill(low.begin(), low.end(), -1);
    std::fill(parent.begin(), parent.end(), -1);
    std::fill(articulationPoints.begin(), articulationPoints.end(), false);
    time = 0;

    // 从每个未访问的顶点开始DFS
    for (int i = 0; i < V; i++) {
        if (!visited[i]) {
            DFS(i);
        }
    }
}

std::vector<int> Graph::getArticulationPoints() const
{
    std::vector<int> result;
    for (int i = 0; i < V; i++) {
        if (articulationPoints[i]) {
            result.push_back(i);
        }
    }
    return result;
}

int Graph::countArticulationPoints() const
{
    int count = 0;
    for (bool isAP : articulationPoints) {
        if (isAP)
            count++;
    }
    return count;
}

bool Graph::isArticulationPoint(int vertex) const
{
    if (vertex < 0 || vertex >= V)
        return false;
    return articulationPoints[vertex];
}

int Graph::getEdgeCount() const
{
    int count = 0;
    for (int i = 0; i < V; i++) {
        count += adj[i].size();
    }
    return count / 2; // 无向图，每条边被计算两次
}

void Graph::printGraph() const
{
    for (int i = 0; i < V; i++) {
        std::cout << "顶点 " << i << ": ";
        for (int neighbor : adj[i]) {
            std::cout << neighbor << " ";
        }
        std::cout << std::endl;
    }
}

bool Graph::convertToNonArticulation(int vertex)
{
    if (!isArticulationPoint(vertex)) {
        return false; // 不是关节点，无需改造
    }

    // 找到所有使得该顶点成为关节点的子节点
    for (int v : adj[vertex]) {
        // 尝试连接v到vertex的另一个邻居
        for (int neighbor : adj[vertex]) {
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
