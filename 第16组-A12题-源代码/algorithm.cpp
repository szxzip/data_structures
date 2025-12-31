#include "header.h"

void Graph::DFS(int u)
{
    visited[u] = true; // 把现在到达的节点设置为已经访问过
    time++; // 时间增加，用来判断经过的先后顺序
    disc[u] = low[u] = time; // disc为本节点到达的时间点，low为本节点能连通到的最早经过节点的经过时间，用以判断环路
    int children = 0; // 初始化，将当前节点的邻居（孩子）数量设置为0

    for (int i = 0; i < adj[u].size(); i++) { // 深度优先遍历
        int v = adj[u][i];
        if (!visited[v]) { // 找到尚未经过的邻居节点
            children++; // 记录两个节点的关系
            parent[v] = u;
            DFS(v); // 对该邻居递归调用

            low[u] = my_min(low[u], low[v]); // 通过孩子可能到达更早经过的节点，因此更新值（my_min：自定义模板函数）

            if (parent[u] == -1 && children > 1) { // 对于根节点，用孩子的数量进行判断即可，>=2为关节点
                articulationPoints[u] = true;
            }

            if (parent[u] != -1 && low[v] >= disc[u]) { // 对于非根节点，若其子节点无法回到比u更早经过的节点,代表没有环路，为关节点
                articulationPoints[u] = true;
            }
        } else if (v != parent[u]) { // 处理环路情况，即通过环路回到了更早已经经过的邻居节点的情况
            low[u] = my_min(low[u], disc[v]);
        }
    }
    cout << "深度优先搜索：完成！" << endl;
}

void Graph::findArticulationPoints()
{ // 查找并标记关节点
    // 重置所有数组
    visited.assign(V, false);
    disc.assign(V, 0);
    low.assign(V, 0);
    parent.assign(V, -1);
    articulationPoints.assign(V, false);
    time = 0;

    // 对每个未访问的顶点调用DFS，虽然有递归，但要防止图一开始就不联通的情况
    for (int i = 0; i < V; i++) {
        if (!visited[i]) {
            DFS(i);
        }
    }
    cout << "查找并标记关节点：完成！" << endl;
}

vector<int> Graph::getArticulationPoints() const // 获取关节点列表
{
    vector<int> result;
    for (int i = 0; i < V; i++) { // 拿articulation数组直接遍历
        if (articulationPoints[i]) {
            result.push_back(i);
        }
    }
    cout << "获取关节点列表：完成！" << endl;
    return result;
}

int Graph::countArticulationPoints() const
{ // 统计关节点数量
    int count = 0;
    for (int i = 0; i < V; i++) { // 拿articulation数组直接遍历
        if (articulationPoints[i]) {
            count++;
        }
    }
    cout << "统计关节点数量：" << count << endl;
    return count;
}

bool Graph::isArticulationPoint(int vertex) const
{ // 判断是否为关节点
    if (vertex >= 0 && vertex < V) {
        cout << "判断是否为关节点：是。" << endl;
        return articulationPoints[vertex];
    }
    cout << "判断是否为关节点：否。" << endl;
    return false;
}
