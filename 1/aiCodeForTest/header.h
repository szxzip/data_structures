#ifndef HEADER_H
#define HEADER_H

#include <cmath>
#include <ctime>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

// 图类定义
class Graph {
private:
    int V; // 顶点数
    std::vector<std::vector<int>> adj; // 邻接表
    std::vector<bool> visited;
    std::vector<int> disc;
    std::vector<int> low;
    std::vector<int> parent;
    std::vector<bool> articulationPoints;
    int time;

    void DFS(int u);

public:
    Graph(int vertices);

    // 基本操作
    void addEdge(int u, int v);
    void removeEdge(int u, int v);
    bool edgeExists(int u, int v) const;
    int getVertexCount() const { return V; }
    int getEdgeCount() const;
    void printGraph() const;
    bool convertToNonArticulation(int vertex);

    // 关节点相关
    void findArticulationPoints();
    std::vector<int> getArticulationPoints() const;
    int countArticulationPoints() const;
    bool isArticulationPoint(int vertex) const;

    // 辅助函数
    const std::vector<std::vector<int>>& getAdjacencyList() const { return adj; }
    void clear();
    void resize(int vertices);

    // 手动输入
    void inputFromConsole();
};

// 图输入/输出类
class GraphIO {
public:
    static Graph* createFromFile(const std::string& filename);
    static Graph* createRandom(int vertices, double density = 0.3);
    static bool saveToFile(const Graph& graph, const std::string& filename);
};

// 工具函数
namespace Utils {
template <typename T>
int find(const std::vector<T>& vec, const T& value)
{
    for (size_t i = 0; i < vec.size(); i++) {
        if (vec[i] == value)
            return i;
    }
    return -1;
}

template <typename T>
T min(T a, T b)
{
    return a < b ? a : b;
}

int randomInt(int min, int max);
double randomDouble();
}

#endif // HEADER_H
