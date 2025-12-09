#ifndef GRAPH_ARTICULATION_H
#define GRAPH_ARTICULATION_H

#include <algorithm>
#include <cmath>
#include <ctime>
#include <fstream>
#include <functional>
#include <iostream>
#include <memory>
#include <queue>
#include <random>
#include <sstream>
#include <stack>
#include <string>
#include <vector>

// 图类定义
class Graph {
private:
    int V; // 顶点数
    std::vector<std::vector<int>> adj; // 邻接表
    std::vector<bool> visited;
    std::vector<int> disc; // 发现时间
    std::vector<int> low; // 最早可访问的祖先
    std::vector<int> parent;
    std::vector<bool> articulationPoints;
    int time;

    void DFS(int u);

public:
    Graph(int vertices);

    // 图操作
    void addEdge(int u, int v);
    void removeEdge(int u, int v);
    bool edgeExists(int u, int v) const;

    // 关节点求解
    void findArticulationPoints();
    std::vector<int> getArticulationPoints() const;
    int countArticulationPoints() const;
    bool isArticulationPoint(int vertex) const;

    // 改造关节点为非关节点的功能
    bool convertToNonArticulation(int vertex);

    // 图信息
    int getVertexCount() const { return V; }
    int getEdgeCount() const;
    void printGraph() const;
    const std::vector<std::vector<int>>& getAdjacencyList() const { return adj; }

    // 清除图
    void clear()
    {
        adj.clear();
        V = 0;
        articulationPoints.clear();
    }

    // 重置图大小
    void resize(int vertices)
    {
        V = vertices;
        adj.resize(V);
        articulationPoints.resize(V, false);
    }
};

// 图输入/输出类
class GraphIO {
public:
    static std::unique_ptr<Graph> createFromFile(const std::string& filename);
    static std::unique_ptr<Graph> createRandom(int vertices, double density = 0.3);
    static bool saveToFile(const Graph& graph, const std::string& filename);
};

// 函数声明，用于在main.cpp中调用
void update_graph_display();

#endif // GRAPH_ARTICULATION_H
